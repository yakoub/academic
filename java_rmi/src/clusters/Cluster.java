package clusters;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.Vector;

public interface Cluster extends Remote {
//    public void setRmiIP(String ip)throws RemoteException;
    public void add(String line)throws RemoteException;        
    public void newRequest(String RequestID)throws RemoteException;
    public Vector getUserRatings(int i)throws RemoteException;
 
}
