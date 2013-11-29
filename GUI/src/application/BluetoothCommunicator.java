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

	public int[] receiveMessage(int length) throws IOException {
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

	public void sendMessageToDevice(int[] message) throws IOException {
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		
		System.out.println("Sent: " + Arrays.toString(message));
		for (int i = 0; i < message.length; i++) {
			os.write(message[i]);
		}
		os.flush();
	}
	
	/*public void broadcastCommand(String str) {
		String url = this.service.getConnectionURL(ServiceRecord.AUTHENTICATE_NOENCRYPT, false);

		StreamConnection conn = null;

		try {
			System.out.println("Sending command to " + url);

			conn = (StreamConnection) Connector.open(url);
			DataInputStream din = conn.openDataInputStream();

			byte[] b = new byte[2];
			for (int i = 0; i < 20; i++) {
				int s = din.read(b);
				System.out.println(b[0]);
				System.out.println(b[1]);
			}

			din.close();
			conn.close();

			System.out.println("Sent. Connection Closed.");

		} catch (Exception e) {
			System.out.println("Failed to connect to " + url);
			e.printStackTrace();
		}
	}*/
}
