package clusters;
import system.Request;

import java.util.Vector;
import java.rmi.RemoteException;
import java.rmi.Naming;
import java.rmi.server.UnicastRemoteObject;

public class SuperUser extends UnicastRemoteObject implements Cluster{
    
    Vector Users;
    Byte ClusterHook;
    Vector Requests;

    public SuperUser()throws RemoteException{
	    super();
	    ClusterHook=new Byte("5");
	    Users=new Vector();
	    Requests=new Vector();
    }

    public void add(String Line)throws RemoteException{
        User U=new User(Line);
        U.hook(ClusterHook);
            synchronized(Users){
                    Users.add(U);
                    U.start();
            }
    }
    public void newRequest(String RequestID)throws RemoteException{
	    try{
	    	Request R=(Request)Naming.lookup(RequestID);
            	synchronized(Requests){
                	    Requests.add(R);
            	}
	    }catch(Exception e){}
	    handleRequest();
    }
    public Vector getUserRatings(int i)throws RemoteException{
	    return ((User)Users.get(i)).Ratings;
    }
    void handleRequest()throws RemoteException{
	    if(RequestsIsEmpty()) return ;

	    Request Req=GetRequest();                    
	    Competition newComp=new Competition(Req);
	    for(int i=0;i<Users.size();i++)
                    ((User)Users.get(i)).EnterCompetition(newComp);                               
	    synchronized(ClusterHook){                    
                    ClusterHook.notifyAll();
	    }
	    synchronized(newComp){                    
		    while(newComp.registered<Users.size()){
                           try{
				   newComp.wait();
			   }catch(Exception e){}
		    }
	    }  
	    try{
	    	Req.Register(newComp.localFinalists);
	    }catch(Exception e){}
    }
    boolean RequestsIsEmpty()throws RemoteException{
        boolean isEmpty;
        synchronized(Requests){
            isEmpty=Requests.isEmpty();
        }
        return isEmpty;
    }
    Request GetRequest()throws RemoteException{
        Request tmp;
        synchronized(Requests){
            tmp= (Request)Requests.get(0);
            Requests.remove(0);            
        }
        return tmp;
    }

                    
}	
