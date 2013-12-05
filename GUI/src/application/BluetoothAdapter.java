package application;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
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
	private void setup() throws IOException {
		if (! DEBUG) {
			conn = (StreamConnection) Connector.open(this.bluetoothUrl);
			os = new DataOutputStream(conn.openOutputStream());
			is = conn.openDataInputStream();
		}
		isSetup = true;
		receiver.communicationGained();
	}

	/**
	 * Callback class for users of this class.
	 */
	public interface IMessageReceiver {
		public void receiveMessage(int header, int data);
		public void communicationGained();
		public void communicationLost();
	}

	/**
	 * Sends a message to the Bluetooth server.
	 * @param header The header byte to send
	 * @param data The data byte to send
	 */
	public void sendMessage(int header, int data) {
		if (! DEBUG) {
			int[] b = new int[2];
			b[0] = header;
			b[1] = data;
			if (isSetup) {
				try {
					sendMessageToDevice(b);
				} catch (IOException e) {
					this.teardown();
					receiver.communicationLost();
				}
			}
		} else {
			System.out.print("Sent: [0x");
			System.out.print(header & 0xFF);
			System.out.print(", 0x");
			System.out.print(data & 0xFF);
			System.out.println("]");
		}
	}

	/**
	 * Blocks until exit() is called. Issues a callback when the connection is lost or a message is received. 
	 * Tries to reconnect if the connection was lost.
	 */
	public void receiveMessages() {
		while (running) {
			// Generate fake data if debug
			if (DEBUG) {
				
				if (!isSetup) {
					try {
						this.setup();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
				
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

				receiver.receiveMessage(header, data);
			} else {
				if (isSetup) {
					try {
						final int[] ret = receiveMessage(2);

						receiver.receiveMessage(ret[0], ret[1]);
					} catch (IOException e) {
						this.teardown();
						receiver.communicationLost();
					}
				} else {
					try {
						this.setup();
					} catch (IOException e) {
						this.teardown();
					}
				}

			}
		}
		this.teardown();
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
		if (! DEBUG && isSetup) {
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
			System.out.print("Received: [0x");
			System.out.print(ret[0]);
			System.out.print(", 0x");
			System.out.print(ret[1]);
			System.out.println("]");

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
			System.out.print("Sent: [0x");
			System.out.print(message[0]);
			System.out.print(", 0x");
			System.out.print(message[1]);
			System.out.println("]");
			for (int i = 0; i < message.length; i++) {
				os.write(message[i]);
			}
			os.flush();
		}
	}
}
