package system;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;

public class remoteHook 
extends UnicastRemoteObject implements HookInterface{
	int count;
	int registered;

	public remoteHook()throws RemoteException{
		super();
	}
	public remoteHook(int i)throws RemoteException{
		count=i;
		registered=0;
	}
	public boolean notReady()throws RemoteException{
               
		if(registered>=count)
			return false;
                
		synchronized(this){
                    try{
			wait();
                    }catch(InterruptedException e){}
		}
		if(registered<count)
			return true;
		else
			return false;
	}
	public void register()throws RemoteException{
		synchronized(this){
			registered++;
			notify();
		}
	}
         public int getnum()throws RemoteException{
             return count;
         } 
}
			
