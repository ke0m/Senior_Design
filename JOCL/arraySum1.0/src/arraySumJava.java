
public class arraySumJava {
	
	
	public float[][] javaCPUArraySum(float[][] array1, float[][] array2, int iters){
		
		 int n1 = array1.length;
		 int n2 = array1[0].length;
		 
		 float[][] array3 = new float[n1][n2];
		 
		 
		for (int i = 0; i<n1; i++){
			for (int j=0; j<n2; j++){
				array3[i][j] = array1[i][j] + array2[i][j];
			for (int k=0; k<iters; k++){
				array3[i][j] += array2[i][j];
				}
			
			}	
			
		}
	
		return array3;
	
	}
	
	
	public static void main(String args[]){
		
		arraySumJava test = new arraySumJava();
		
		int n1 = 2;
		int n2 = 3;
		
		float[][] aa = new float[n1][n2];
		float[][] bb = new float[n1][n2];
		float[][] cc = new float[n1][n2];
		
		
		for(int i = 0; i<n1; i++){
			for(int j = 0; j<n2; j++){
				
				aa[i][j] = i + j;
				bb[i][j] = i + j;
				
			}
			
		}

		cc = test.javaCPUArraySum(aa, bb, 250);
		
		for(int i = 0; i < n1; i++){
            for(int j = 0; j < n2; j++){
    
                System.out.print(cc[i][j]+" ");
            }   
            System.out.print("\n");
        }   
	
	
	
	
	}
	
	
	
}
