package clusters;
import java.util.Vector;

public class User extends Thread{
	public Vector Ratings;
	Vector CompetitionQ;        
        Byte ClusterHook;
        public int Id,SuperId;

	public User(String str){
            Ratings=new Vector();
            CompetitionQ=new Vector();
            String numbers[]=str.split(",");
            for(int i=0;i<numbers.length;i++)
               Ratings.add(Float.valueOf(numbers[i]));
	}
        
        public synchronized void hook(Byte Hook){
            ClusterHook=Hook;            
        }

        public void EnterCompetition(Competition C){
            synchronized(CompetitionQ){
                CompetitionQ.add(C);
            }
        }
	public void run(){            
            while(true){
                try{
                    while(CompetitionQIsEmpty()){
                        synchronized(ClusterHook){
                            ClusterHook.wait();			                            
			}
                    }
                    Competition Comp;
                    Comp=getCompetition();
                    Vector Active=Comp.Active;
                    int Item=Comp.itemIndex;
                    if(Item <= Ratings.size()-1){
                        float Pred[]=calculate(Active);            
                        Pred[0]=((Float)Ratings.get(Item)).floatValue();                     
                        Comp.Candidate(Pred);
                    }
                    else{
                        Comp.ExitCompetition();
                    }
                    if(isInterrupted()) break;
                         
                }catch(InterruptedException e){
                        return ;
                }
            }
	}
        
        boolean CompetitionQIsEmpty(){
            boolean state;            
            synchronized(CompetitionQ){
                 state=CompetitionQ.isEmpty();
            }
            return state;
        }
        Competition getCompetition(){
            Competition tmp;
            synchronized(CompetitionQ){
                 tmp=(Competition)CompetitionQ.get(0);
                 CompetitionQ.remove(0);
            }
            return tmp;
        }
        
	float[] calculate(Vector Active){ 
            if(Active.size()!=Ratings.size() ) return null;
            float Pred[]=new float[2];
            Pred[0]=0;Pred[1]=0;
            for(int i=0;i<Ratings.size()&&i<Active.size();i++){
                float tmp=((Float)Ratings.get(i)).floatValue()-
			  ((Float)Active.get(i)).floatValue();
                tmp*=tmp;
                Pred[0]+=tmp;      
            }
            Pred[1]/=Ratings.size();
            Pred[1]=(float)Math.sqrt(Pred[0]);
            return Pred;
        }
}
		
