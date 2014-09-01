import sys;

from   java.awt       import BorderLayout;
from   java.lang      import Runnable;
from   java.util      import Calendar;

from   javax.swing    import JButton;
from   javax.swing    import JFrame;
from   javax.swing    import JLabel;
from   javax.swing    import SwingUtilities;

class StopWatch( Runnable ) :
  def __init__( self ) :
    self.frame = JFrame( 'StopWatch' ,
                         defaultCloseOperation = JFrame.EXIT_ON_CLOSE );
    self.start = JButton( 'Start', actionPerformed = self.start );
    self.frame.add( self.start, BorderLayout.WEST );
    self.stop  = JButton( 'Stop' , actionPerformed = self.stop  );
    self.frame.add( self.stop, BorderLayout.EAST );
    self.label = JLabel( ' ' * 45 );
    self.frame.add( self.label, BorderLayout.SOUTH );
    self.frame.pack();

  def start( self, event ) :
    self.started = Calendar.getInstance().getTimeInMillis();
    self.label.setText( 'Running' );

  def stop( self, event ) :
    elapsed = Calendar.getInstance().getTimeInMillis() - self.started;
    self.label.setText( 'Elapsed: %.2f seconds' % ( float( elapsed ) / 1000.0 ) );

  def run( self ) :
    self.frame.setVisible( 1 );   # Have the application make itself visible


