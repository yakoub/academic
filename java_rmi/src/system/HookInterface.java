package system;
import java.rmi.Remote;
import java.rmi.RemoteException;

public interface HookInterface extends Remote{
    public boolean notReady()throws RemoteException;
    public void register()throws RemoteException;
    public int getnum()throws RemoteException;
}
			
