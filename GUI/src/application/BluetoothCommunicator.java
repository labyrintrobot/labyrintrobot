package application;

import java.io.IOException;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.obex.ClientSession;
import javax.obex.HeaderSet;
import javax.obex.Operation;
import javax.obex.ResponseCodes;

public class BluetoothCommunicator {

	private ClientSession clientSession;
	private Operation putOperation;
	private OutputStream os;

	public void setup(String serverURL) {
		System.out.println("Connecting to " + serverURL);

		try {
			clientSession = (ClientSession) Connector.open(serverURL);
			HeaderSet hsConnectReply = clientSession.connect(null);
			if (hsConnectReply.getResponseCode() != ResponseCodes.OBEX_HTTP_OK) {
				System.out.println("Failed to connect");
				return;
			}

			HeaderSet hsOperation = clientSession.createHeaderSet();
			hsOperation.setHeader(HeaderSet.NAME, "Hello.txt");
			hsOperation.setHeader(HeaderSet.TYPE, "text");

			// Create PUT Operation
			putOperation = clientSession.put(hsOperation);
			os = putOperation.openOutputStream();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void teardown() {
		try {
			putOperation.close();

			clientSession.disconnect(null);

			clientSession.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void sendMessageToDevice(String serverURL) {
		try {
			// Send some text to server
			byte data[] = "Hello World !!!".getBytes("iso-8859-1");
			os.write(data);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
