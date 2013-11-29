package application;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

public class BluetoothCommunicator {

	private boolean isSetup = false;
	
	private StreamConnection conn;
	private DataOutputStream os;
	private InputStream is;

	public void setup(String bluetoothUrl) throws IOException {
		
		if (isSetup) {
			throw new IllegalStateException("Already set up");
		}
		isSetup = true;

		try {
			conn = (StreamConnection) Connector.open(bluetoothUrl);
			os = new DataOutputStream(conn.openOutputStream());
			is = conn.openDataInputStream();

		} catch (Exception e) {
			System.out.println("Failed to connect to " + bluetoothUrl);
			e.printStackTrace();
		}
	}

	public void teardown() throws IOException {
		
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		isSetup = false;
		
		os.close();
		is.close();
		conn.close();
	}

	public synchronized int[] receiveMessage(int length) throws IOException {
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		
		int[] ret = new int[length];
		for (int i = 0; i < length; i++) {
			ret[i] = is.read();
		}
		System.out.println("Received: " + Arrays.toString(ret));
		
		return ret;
	}

	public synchronized void sendMessageToDevice(int[] message) throws IOException {
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		
		System.out.println("Sent: " + Arrays.toString(message));
		for (int i = 0; i < message.length; i++) {
			os.write(message[i]);
		}
		os.flush();
	}
}
