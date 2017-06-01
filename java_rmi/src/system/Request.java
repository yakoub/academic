package system;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.Vector;

public interface Request extends Remote{
	public Vector getActiveVector()throws RemoteException;
	public int getItemIndex()throws RemoteException;
	public int getSampleSize()throws RemoteException;
	public void Register(Vector Candidates)
				throws RemoteException;
	
}
