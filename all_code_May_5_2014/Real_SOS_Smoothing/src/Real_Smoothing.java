import org.jocl.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.Random;

import edu.mines.jtk.mosaic.*;
import edu.mines.jtk.util.ArrayMath;
import edu.mines.jtk.util.Stopwatch;

import edu.mines.jtk.dsp.*;
import edu.mines.jtk.dsp.LocalSmoothingFilter;



public class Real_Smoothing {
	
	public static void main(String args[])
	{
		Random rand = new Random();
		Scanner in  = new Scanner(System.in);
		//SimplePlot plot = new SimplePlot();
		PlotPanel panel1 = new PlotPanel(1,1);
		PlotPanel panel2 = new PlotPanel(1,1);
	
		
		int n1 = 65;
		int n2 = 25;
		int dims = n1*n2;
		
		LocalSmoothingFilter smooth_cpu = new LocalSmoothingFilter();
		
		float[][] r = new float[n1][n2];
		float[][] s = new float[n1][n2];
		
		for(int i = 0; i < n1; i++)
		{
			for(int j = 0; j < n2; j++)
			{
				r[i][j] = rand.nextFloat();
				s[i][j] = 0.0f;
				
			}
		}
		
		smooth_cpu.apply(r, s);
		
		PixelsView pix1 = panel1.addPixels(r);
		panel1.addTitle("Input Image");
		panel1.addColorBar();
		PixelsView pix2 = panel2.addPixels(s);
		panel2.addTitle("CPU Output");
		panel2.addColorBar();
		
		
		PlotFrame frame1 = new PlotFrame(panel1);
		PlotFrame frame2 = new PlotFrame(panel2);

		
		frame1.setVisible(true);
		frame2.setVisible(true);
		
//		plot.addTitle("Input Image");
//		plot.addColorBar();
//		plot.asPixels(r);
//		plot.asPixels(s);


		frame1.paintToPng(50, 20, "inputimage.png");
		frame1.paintToPng(50, 20, "cpusmoothimg.png");
		
		
	}

}
