
import java.util.Vector;
import java.util.Stack;
import java.util.Iterator;
import java.rmi.Naming;

import java.io.*;
import java.net.*;

import javax.servlet.*;
import javax.servlet.http.*;

public class RecommenderSystem extends HttpServlet{
	static final int sampleSize=100;
	static Cluster Clusters[];
	HookInterface SystemHook;
	static String rmiIP;
	
	//---------------------------------------------
        public void init()throws ServletException{
		
		String[] param=new String[3];
		param[0]=getInitParameter("rmiIP");
		param[1]=getInitParameter("clustersNum");
		param[2]=getInitParameter("dataFile");
		if(param[0]==null||param[0]==""||
			param[1]==null||param[1]==""||
		param[2]==null||param[2]=="")
                throw new ServletException("Wrong init Param");
		
		rmiIP=param[0]+"/";
		int clusters=Integer.parseInt(param[1]);	    
		String file=param[2];
		log("parameters "+param[0]+" , "+param[1]+" , "+param[2]);
		//---------------------------
		try{
			SystemHook=new remoteHook(clusters); 
                        log(rmiIP+"SystemHook");
			Naming.rebind(rmiIP+"SystemHook",SystemHook);
			log("hook registered");
			while(SystemHook.notReady());
			log("hook released");
			Clusters=new Cluster[clusters]; 
			for(int i=0;i<clusters;i++){
				String name=rmiIP+"Cluster"+Integer.toString(i);
				Clusters[i]=(Cluster)Naming.lookup(name);                   
			}
		}catch(Exception e){
			throw new ServletException(e.getMessage()) ;
		}
		ReqIDHandler.freeRequests=new Stack();
		ReqIDHandler.TopID=0;
		log("building clusters");
		buildClusters(file);
		log("clustered build");
	}
	//---------------------------------------------
	void buildClusters(String file){
		try{
			FileReader fin=new FileReader(file); 
			BufferedReader I=new BufferedReader(fin);
			String line=I.readLine();
			int clusterIndex=0;
			while(line!=null){
				Clusters[clusterIndex].add(line);
				clusterIndex=(clusterIndex+1)%Clusters.length;
				line=I.readLine();
			}                
		}catch(IOException e){
			System.out.println(e.getMessage()); 
			return;
		}                
	}
	
	//---------------------------------------------
	
	public void processRequest(HttpServletRequest request, HttpServletResponse response)
	throws ServletException, IOException {
		
		String[] param=new String[2];
		param[0]=request.getParameter("UserID");
		param[1]=request.getParameter("ItemIndex");
		
		if(param[0]==null||param[0]==""||
		param[1]==null||param[1]=="")  {
			log("wrong parameters");
			response.sendError(440,"Wrong request Param");
		}
		
		int Item=Integer.parseInt(param[0]);
		int userID=Integer.parseInt(param[1]);	
		
		response.setContentType("text/html;charset=UTF-8");
		PrintWriter out = response.getWriter();	
		String RequestID=ReqIDHandler.getRequestID();
		
		
                //creat new request
		int cluster=userID%Clusters.length;
		int user=userID/Clusters.length;
		try{
			Vector Active=(Clusters[cluster]).getUserRatings(user);
			Request R=new RequestImpl(Item,Active,sampleSize,Clusters.length);
			Naming.rebind(RequestID,R);
			
			//pass request to Clusters
			for(int i=0;i<Clusters.length;i++)
				Clusters[i].newRequest(RequestID);
			
			//wait for Request to be finished by all Clusters
                        log("waiting for Request");
			while(((RequestImpl)R).notFinished());
			Naming.unbind(RequestID);
			ReqIDHandler.add(RequestID);
                        log("Request done");
			
			//get the best sampleSize user Ratings 
			Vector Finalists=Finals( ((RequestImpl)R).ClusterCandidates);
			
			//calculate Prediction
			float Pred[]=new float[2];
			Pred[0]=0;Pred[1]=0;
			for(int i=0;i<Finalists.size();i++){
				float current[]=(float[])Finalists.get(i);
				float tmp=current[0]*(1-current[1]);
				Pred[0]+=tmp;
				Pred[1]+=1-current[1];
			}
			Pred[0]/=Pred[1];
			Pred[1]=Item;
			
			out.println("<html>");
			out.println("<head>");
			out.println("<title>Servlet RecommenderSystem</title>");
			out.println("</head>");
			out.println("<body>");
			out.println("<center><h1> Prediction" + Pred[0] + "</h1></center>");
			out.println("</body>");
			out.println("</html>");
			
		}catch(Exception e){}
        }
	
	public void doGet(HttpServletRequest request, HttpServletResponse response)
	throws ServletException, IOException {
		processRequest(request, response);
	}
	
	public void doPost(HttpServletRequest request, HttpServletResponse response)
	throws ServletException, IOException {
		processRequest(request, response);
	}
	
	public String getServletInfo() {
		return "CF Recommendation";
	}
	
	//---------------------------------------------
        //every Cluster registers a group of candidates 
        //for the final Prediction sample , this method
        //makes a sampleSize Vector from all the cadidates
        Vector Finals(Vector Candidates){
		Vector Final=new Vector();
                
                //Print(Candidates);
                
		while(Final.size() <sampleSize && !Candidates.isEmpty()){
			//  Print2(Candidates.get(0));
			float Pred[]=(float[])((Vector)(Candidates.get(0))).get(0);
			float CompPred[];
			int winner=0;
			for(int i=0;i<Candidates.size();i++){
				if(((Vector)Candidates.get(i)).isEmpty() ) 
					Candidates.remove(i);
				else{
					CompPred=(float[])((Vector)Candidates.get(i)).get(0);                            
					if(Pred[1]>CompPred[1]){
						Pred=CompPred;
						winner=i;
					}
				}
			}
			((Vector)Candidates.get(winner)).remove(0);
			if(((Vector)Candidates.get(winner)).isEmpty())
				Candidates.remove(winner);
			Final.add(Pred);
		}
		return Final;
        }
        
}

//================================================
class ReqIDHandler{
	public static Stack freeRequests;
	public static int TopID;
	public static synchronized String getRequestID(){
		int id;
		if(freeRequests.isEmpty()){
			id=TopID++;
		}else{
			return (String)freeRequests.pop();
		}
		String tmp=new String(RecommenderSystem.rmiIP+"Request");
		tmp+=Integer.toString(id);
		return tmp;
	}
	public static synchronized void add(String Id){
		freeRequests.push(Id);
	}
}
//================================================

