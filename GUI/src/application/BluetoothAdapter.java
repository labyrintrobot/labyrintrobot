package application;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Random;

import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

/**
 * Abstraction from the Bluetooth layer.
 * @author Emil Berg
 *
 */
public class BluetoothAdapter {

	// true to generate data
	private static final boolean DEBUG = false;
	
	private static final DateFormat hourMinuteSecond = new SimpleDateFormat("HH:mm:ss");

	private final Object recLock = new Object();
	private final Object sendLock = new Object();

	private final String bluetoothUrl;
	private final IMessageReceiver receiver;
	private final Random rng = new Random();

	private boolean isSetup = false;
	private boolean running = true;
	private StreamConnection conn;
	private DataOutputStream os;
	private InputStream is;

	public BluetoothAdapter(String bluetoothUrl, IMessageReceiver receiver) {
		this.bluetoothUrl = bluetoothUrl;
		this.receiver = receiver;
	}

	/**
	 * Setups a Bluetooth connection.
	 */
	public void setup() {
		while (!isSetup) {
			try {
				if (! DEBUG) {
					conn = (StreamConnection) Connector.open(this.bluetoothUrl);
					os = new DataOutputStream(conn.openOutputStream());
					is = conn.openDataInputStream();
				}
				isSetup = true;
				receiver.communicationGained();
			} catch (IOException e) {
				// Do nothing. Just try again.
			}
		}
	}

	/**
	 * Callback class for users of this class.
	 */
	public interface IMessageReceiver {
		public void receiveMessage(Header header, int data);
		public void receiveInvalidMessage(int header, int data);
		public void communicationGained();
		public void communicationLost();
	}

	/**
	 * Sends a message to the Bluetooth server.
	 * @param header The header byte to send
	 * @param data The data byte to send
	 */
	public void sendMessage(Header header, int data) {
		
		if (isSetup) {
			System.out.print(hourMinuteSecond.format(new Date()));
			System.out.print(": Sent: [");
			System.out.print(header.name());
			System.out.print(", 0x");
			System.out.print(Integer.toHexString(data));
			System.out.println("]");
			if (! DEBUG) {
				int[] b = new int[2];
				b[0] = header.getIndex();
				b[1] = data;
				while (true) {
					try {
						sendMessageToDevice(b);
						receiver.communicationGained();
						break;
					} catch (IOException e) {
						receiver.communicationLost();
					}
				}
			}
		}
	}

	/**
	 * Blocks until exit() is called. Issues a callback when the connection is lost or a message is received. 
	 * Tries to reconnect if the connection was lost.
	 */
	public void receiveMessages() {
		
		if (!isSetup) {
			throw new IllegalStateException();
		}
		
		while (running) {
			// Generate fake data if debug
			if (DEBUG) {
				
				try {
					Thread.sleep(10);
				} catch (InterruptedException e) {
					// Do nothing
				}

				int headerData = (byte)rng.nextInt(0x0E);
				int data;
				if (headerData == 0x01) {
					data = rng.nextInt(0x0B);
				} else {
					data = rng.nextInt(0xFF);
				}
				rec(headerData, data);
			} else {
				try {
					final int[] ret = receiveMessage(2);
					receiver.communicationGained();
					rec(ret[0], ret[1]);
				} catch (IOException e) {
					receiver.communicationLost();
				}
			}
		}
		this.teardown();
	}
	
	private void rec(int headerData, int data) {
		Header h = Header.fromInt(headerData);
		System.out.print(hourMinuteSecond.format(new Date()));
		if (h == null) {
			System.out.print(": Received: [0x");
			System.out.print(headerData);
			System.out.print(", 0x");
			System.out.print(Integer.toHexString(data));
			System.out.println("]");
			receiver.receiveInvalidMessage(headerData, data);
		} else {
			System.out.print(": Received: [");
			System.out.print(h.name());
			System.out.print(", 0x");
			System.out.print(Integer.toHexString(data));
			System.out.println("]");
			receiver.receiveMessage(h, data);
		}
	}

	/**
	 * Stops listening for messages.
	 */
	public void exit() {
		running = false;
	}

	/**
	 * Cleanup.
	 */
	private void teardown() {
		
		if (!isSetup) {
			throw new IllegalStateException();
		}
		
		if (! DEBUG) {
			try {
				os.close();
				is.close();
				conn.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		this.isSetup = false;
	}

	/**
	 * 
	 * @param length The length of the int array.
	 * @return The received array with the specified length.
	 * @throws IOException If there was a connection error.
	 */
	private int[] receiveMessage(int length) throws IOException {
		synchronized (recLock) {
			int[] ret = new int[length];
			for (int i = 0; i < length; i++) {
				ret[i] = is.read();
				if (ret[i] == -1) {
					throw new IOException();
				}
			}

			return ret;
		}
	}

	/**
	 * Sends a message to the server.
	 * @param message The message to send.
	 * @throws IOException If there was a connection error.
	 */
	private void sendMessageToDevice(int[] message) throws IOException {
		synchronized (sendLock) {
			for (int i = 0; i < message.length; i++) {
				os.write(message[i]);
			}
			os.flush();
		}
	}
}
