package clusters;
import system.Request;
import java.util.Vector;
import java.util.Iterator;

public class Competition{
	public Vector Active;
	public int itemIndex;
	static int sampleSize;
	public int registered;
	public Vector localFinalists;

	public Competition(Request Req){
		registered=0;
		localFinalists=new Vector();
		try{
			Active=Req.getActiveVector();
			itemIndex=Req.getItemIndex();
			sampleSize=Req.getSampleSize();
		}catch(Exception e){}
	}

        public synchronized void ExitCompetition(){
            	registered++;
                notify();
        }
	public synchronized void Candidate(float Pred[]){
                if(localFinalists.isEmpty()){
                    localFinalists.add(Pred);
                }
                else{
                    float tmp[];
                    for(int i=0;i<localFinalists.size();i++){
                        tmp=(float[])localFinalists.get(i);
                        if(tmp[1]>Pred[1]){
                            localFinalists.insertElementAt(Pred,i);
                            break;
                        }
                    }
                    tmp=(float[])localFinalists.get(localFinalists.size()-1);
                    if(tmp[1]>Pred[1])
                        localFinalists.add(Pred);
                }		
		if(localFinalists.size()>sampleSize){
			int i=localFinalists.size();
			localFinalists.remove(i-1);
		}
		registered++;
                notify();
	}
	
}
