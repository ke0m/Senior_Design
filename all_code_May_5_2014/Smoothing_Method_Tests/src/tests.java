import edu.mines.jtk.util.*;
import java.util.Random;
public class tests {
	
	public static void main(String args[]){
		
		Random rand = new Random();
		
		int n1 = 501;
		int n2 = 501;
		boolean check = true;
		float alpha = 18.f;
		
		float[][] x = new float[n1][n2];
		float[][] y = new float[n1][n2];
		float[][] yp = new float[n1][n2];
		
		float[] x1 = new float[n1*n2];
		float[] y1 = new float[(n1+1)*(n2+1)];
		
		for(int i = 0; i < n1; i++)
		{
			for(int j = 0; j < n2; j++)
			{
				x[i][j] = rand.nextFloat();
				y[i][j] = rand.nextFloat();
			}
		}
		
		
		
		CLUtil.packArray(n1,n2,x,x1);
		CLUtil.packArray(n1,n2,y,y1);
		
		float testdot = sdot(x,y);
		float testdot1 = sdot1(n1,n2,x1,y1);
		
		if(testdot1 == testdot)
			System.out.println("Dot Product passed!");
		
		
		
		
//		saxpy(-alpha,x,y);
//		saxpy1(n1,n2,-alpha,x1,y1);
		
		sxpay(-alpha,x,y);
		sxpay1(n1,n2,-alpha,x1,y1);
		
		
		CLUtil.unPackArray(n1, n2, y1, yp);
		
		for(int i = 0; i < n1; ++i)
		{
			for(int j = 0; j < n2; j++)
			{
				if(y[i][j] == yp[i][j])
				{
					continue;
				}
				else
				{
					check = false;
					System.out.println("Check failed at element: " + i + "," + j);
				}
			}
		}
		
		if(check == true)
			System.out.println("Check Passed!");
		else
			System.out.println("Check Failed");
		
	}
	
	public static void saxpy1(int n1, int n2, float a, float[] x, float[] y) {
		for(int i = 0; i < n1*n2; ++i) {
			y[i] += a*x[i];
		}
		  
	  }
	
	
	  // Computes y = y + a*x.
	  //TODO: Write for 1D arrays
	  public static void saxpy(float a, float[][] x, float[][] y) {
	    int n1 = x[0].length;
	    int n2 = x.length;
	    for (int i2=0; i2<n2; ++i2) {
	      float[] x2 = x[i2], y2 = y[i2];
	      for (int i1=0; i1<n1; ++i1) {
	        y2[i1] += a*x2[i1];
	      }
	    }
	  }
	  
	  
	  
	  public static float sdot1(int n1, int n2, float[] x, float[]y){
		  int n = n1*n2;
		  float d = 0.0f;
		  for(int i = 0; i < n; ++i) {
			  d += x[i]*y[i];
		  }
		  return d;
	  }

	  
	  // Returns the dot product x'y.
	  public  static float sdot(float[][] x, float[][] y) {
	    int n1 = x[0].length;
	    int n2 = x.length;
	    float d = 0.0f;
	    for (int i2=0; i2<n2; ++i2) {
	      float[] x2 = x[i2], y2 = y[i2];
	      for (int i1=0; i1<n1; ++i1) {
	        d += x2[i1]*y2[i1];
	      }
	    }
	    return d;
	  }
	  
	  
	  
	  private static void sxpay1(int n1, int n2, float a, float[] x, float[]y) {
		  for(int i = 0; i< n1*n2; ++i) {
			  y[i] = a*y[i] + x[i];
		  }
		  
		  
	  }

	  // Computes y = x + a*y.
	  //TODO: Write for 1D arrays
	  private static void sxpay(float a, float[][] x, float[][] y) {
	    int n1 = x[0].length;
	    int n2 = x.length;
	    for (int i2=0; i2<n2; ++i2) {
	      float[] x2 = x[i2], y2 = y[i2];
	      for (int i1=0; i1<n1; ++i1) {
	        y2[i1] = a*y2[i1]+x2[i1];
	      }
	    }
	  }
	  
	  
	  
}
