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
	
	public void setup(String serverUrl) throws IOException {
		this.bluetoothCommunicator.setup(serverUrl);
	}
	
	public interface IMessageReceiver {
		public void receiveMessage(byte header, int data);
	}
	
	public void sendMessage(byte header, int data) throws IOException {
		byte[] b = new byte[2];
		b[0] = header;
		b[1] = (byte) data;
		this.bluetoothCommunicator.sendMessageToDevice(b);
	}
	
	public void receiveMessages() throws IOException {
		// TODO
		while (running) {
			if (DEBUG) {
				try {
					Thread.sleep(2);
				} catch (InterruptedException e) {
					// Do nothing
				}
				
				final byte header = (byte)rng.nextInt(0x0B);
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
				
			}
		}
		this.bluetoothCommunicator.teardown();
	}
	
	public void exit() {
		running = false;
	}
}
