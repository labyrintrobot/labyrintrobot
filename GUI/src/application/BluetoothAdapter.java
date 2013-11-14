package application;

import java.util.Random;

import javafx.application.Platform;

public class BluetoothAdapter {
	
	private static final boolean DEBUG = true;
	
	private boolean running = true;
	
	private final IMessageReceiver receiver;
	
	private final Random rng = new Random();
	
	public BluetoothAdapter(IMessageReceiver receiver) {
		this.receiver = receiver;
	}
	
	public interface IMessageReceiver {
		public void receiveMessage(byte header, int data);
	}
	
	public void sendMessage(byte header, int data) {
		// TODO
		// TODO CRC?
	}
	
	public void receiveMessages() {
		// TODO
		while (running) {
			if (DEBUG) {
				try {
					Thread.sleep(2);
				} catch (InterruptedException e) {
					// Do nothing
				}
				
				//final byte header = (byte)rng.nextInt(0x0B);
				final byte header = 0x01;
				final int data;
				if (header == 0x01) {
					data = rng.nextInt(0x07);
				} else {
					data = rng.nextInt(0xFF);
				}
				
				// Now, receiveMessage and sendMessage should run on the same thread.
				Platform.runLater(new Runnable() {
					
					@Override
					public void run() {
						receiver.receiveMessage(header, data);
					}
				});
			}
		}
	}
	
	public void exit() {
		running = false;
	}
}
