import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class test{

	/*
	public String readFile(String path, Charset encoding)
		throws IOException;
	
	{

		/*try
		{
			File file = new File(fileName);
    		BufferedReader br = new BufferedReader(new FileReader(file));
			StringBuilder sb = new StringBuilder();
    		word2 = br.readLine();

			while (word2 != null) {
            	sb.append(word2);
            	sb.append("\n");
            	word2 = br.readLine();
        
			}

    		br.close();
    		//test:
    		System.out.println(word2);

		} 
		catch (IOException e)
		{
    		// Something went wrong, eg: file not found
   		 e.printStackTrace();
		}

		
		return word2;


	 	byte[] encoded = Files.readAllBytes(Paths.get(path));
 	
		return encoding.decode(ByteBuffer.wrap(encoded)).toString();

		




	}*/


	public String readFile(String file) throws FileNotFoundException{

		String text = new Scanner( new File(file)).useDelimiter("\\A").next();
		
		return text;
		
		
	}



	public static void main(String args[]) throws FileNotFoundException{


		test work = new test();

		String code = work.readFile("flopstest.cl");

		System.out.println(code);


	}

}
