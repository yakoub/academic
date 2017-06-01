package clusters;
import system.HookInterface;

import java.rmi.Naming;
import java.io.*;

public class StartClusters{
	public static void main(String[] o)throws Exception{
			String rmiIP="//localhost:1099/";
			
			String[] T=Naming.list(rmiIP);
			
			for(int i=0;i<T.length;i++)
				System.out.println(T[i]);
			
                        
			System.out.println("rmi IP="+rmiIP);                        
			
			HookInterface Hook=(HookInterface)Naming.lookup(rmiIP+"SystemHook");
			int clusters=Hook.getnum();
			Cluster Cs[]=new Cluster[clusters];
			int i=0;
		try{
			for(i=0;i<clusters;i++){
				Cs[i]=new SuperUser();
				String name=new String(rmiIP+"Cluster"+
						Integer.toString(i));
				System.out.println("binding "+name);
				Naming.rebind(name,Cs[i]);
				System.out.println("registering "+name);
				Hook.register();
			}
		}catch(Exception e){
			System.out.println(e+" i "+i);
		}
	}
}
