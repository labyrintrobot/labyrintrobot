package application;

import java.io.IOException;
import java.util.Random;

import javafx.application.Platform;

public class BluetoothAdapter {
	
	private static final boolean DEBUG = true;
	
	private boolean running = true;
	
	private final IMessageReceiver receiver;
	
	private final Random rng = new Random();
	
	private final BluetoothCommunicator bluetoothCommunicator;
	
	public BluetoothAdapter(IMessageReceiver receiver) {
		this.receiver = receiver;
		this.bluetoothCommunicator = new BluetoothCommunicator();
	}
	
	public void setup(String bluetoothUrl) throws IOException {
		if (! DEBUG) {
			this.bluetoothCommunicator.setup(bluetoothUrl);
		}
	}
	
	public interface IMessageReceiver {
		public void receiveMessage(int header, int data);
	}
	
	public void sendMessage(int header, int data) throws IOException {
		int[] b = new int[2];
		b[0] = header;
		b[1] = data;
		this.bluetoothCommunicator.sendMessageToDevice(b);
	}
	
	public void receiveMessages() throws IOException {
		while (running) {
			if (DEBUG) {
				try {
					Thread.sleep(10);
				} catch (InterruptedException e) {
					// Do nothing
				}
				
				final byte header = (byte)rng.nextInt(0x0C);
				final int data;
				if (header == 0x01) {
					data = rng.nextInt(0x07);
				} else {
					data = rng.nextInt(0xFF);
				}
				
				Platform.runLater(new Runnable() {
					
					@Override
					public void run() {
						receiver.receiveMessage(header, data);
					}
				});
			} else {
				final int[] ret = this.bluetoothCommunicator.receiveMessage(2);
				
				Platform.runLater(new Runnable() {
					
					@Override
					public void run() {
						receiver.receiveMessage(ret[0], ret[1]);
					}
				});
				this.bluetoothCommunicator.teardown();
			}
		}
	}
	
	public void exit() {
		running = false;
	}
}
