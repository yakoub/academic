package system;
import java.util.Vector;
import java.util.Iterator;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;

public class RequestImpl extends UnicastRemoteObject
			 implements Request{
	int sampleSize;
	int registered;
	int itemIndex;
	int clusters;
	Vector Items;
	public Vector ClusterCandidates;
	

	public boolean notFinished()throws RemoteException{
		if(registered>=clusters)
			return false;
		synchronized(this){
			try{
				wait();
			}catch(InterruptedException e){}
		}
		if(registered>=clusters)
			return true;
		else
			return false;
	}
	public RequestImpl(int item,Vector Items,int sampleSize,
			int clusters)throws RemoteException{
		itemIndex=item;
		this.Items=Items;
		registered=0;
                ClusterCandidates=new Vector();
		this.sampleSize=sampleSize;
		this.clusters=clusters;
	}
	public Vector getActiveVector()throws RemoteException{
		return Items;
	}
	public int getItemIndex()throws RemoteException{
		return itemIndex;
	}
	public int getSampleSize()throws RemoteException{
		return sampleSize;
	}

	public  void Register(Vector Candidates)
				throws RemoteException{
		synchronized(this){            		
            		ClusterCandidates.add(Candidates);	
            		registered++;
	   	 	notify();
		}
	}

	
}
