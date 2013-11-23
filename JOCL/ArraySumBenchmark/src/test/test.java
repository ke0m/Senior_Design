import edu.mines.jtk.util.*;

public class test {


	public static void main(String args[]){

		final float[] array1 = {1,2,3,4,5,6,7,8,9,10};

		/*for (int i = 0; i<array1.length; ++i) {
		  System.out.println(array1[i]);
		}*/

		Parallel.loop(10,new Parallel.LoopInt() {
		  public void compute (int i) {
		    System.out.println(i);
		  }
	  });
	
	
	}



}


