package application;

import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

import javax.bluetooth.DataElement;
import javax.bluetooth.DeviceClass;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.RemoteDevice;
import javax.bluetooth.ServiceRecord;
import javax.bluetooth.UUID;
import javax.microedition.io.Connector;
import javax.obex.ClientSession;
import javax.obex.HeaderSet;
import javax.obex.Operation;
import javax.obex.ResponseCodes;

public class BluetoothDiscoveryListener implements DiscoveryListener {
	
	private static final boolean DEBUG = true;

	private static final Object lock = new Object();
	
	private final List<RemoteDevice> devices;
	private final IBluetoothDeviceDiscovered deviceDiscoveredCallback;

	public BluetoothDiscoveryListener(IBluetoothDeviceDiscovered deviceDiscoveredCallback) {
		devices = new ArrayList<>();
		this.deviceDiscoveredCallback = deviceDiscoveredCallback;
	}

	public void start() {

		try {
			LocalDevice localDevice = LocalDevice.getLocalDevice();
			DiscoveryAgent agent = localDevice.getDiscoveryAgent();
			agent.startInquiry(DiscoveryAgent.GIAC, this);

			try {
				synchronized (lock) {
					lock.wait();
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
				return;
			}
			
			if (DEBUG) {
				System.out.println("Device Inquiry Completed. ");
			}

			UUID[] uuidSet = new UUID[1];
			uuidSet[0] = new UUID(0x1105); // OBEX Object Push service

			int[] attrIDs = new int[] { 0x0100 // Service name
			};

			for (RemoteDevice device : devices) {
				agent.searchServices(attrIDs, uuidSet, device, this);

				try {
					synchronized (lock) {
						lock.wait();
					}
				} catch (InterruptedException e) {
					e.printStackTrace();
					return;
				}

			}
			if (DEBUG) {
				System.out.println("Service search finished.");
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	public void deviceDiscovered(RemoteDevice btDevice, DeviceClass arg1) {
		String name;
		try {
			name = btDevice.getFriendlyName(false);
		} catch (Exception e) {
			name = btDevice.getBluetoothAddress();
		}

		devices.add(btDevice);
		
		deviceDiscoveredCallback.deviceDiscovered(name);
		
		if (DEBUG) {
			System.out.println("Device found: " + name);
		}
	}

	@Override
	public void inquiryCompleted(int arg0) {
		synchronized (lock) {
			lock.notify();
		}
	}

	@Override
	public void serviceSearchCompleted(int arg0, int arg1) {
		synchronized (lock) {
			lock.notify();
		}
	}

	@Override
	public void servicesDiscovered(int transID, ServiceRecord[] servRecord) {
		for (int i = 0; i < servRecord.length; i++) {
			String url = servRecord[i].getConnectionURL(
					ServiceRecord.NOAUTHENTICATE_NOENCRYPT, false);
			if (url == null) {
				continue;
			}
			DataElement serviceName = servRecord[i].getAttributeValue(0x0100);
			if (serviceName != null) {
				System.out.println("Service " + serviceName.getValue()
						+ " found " + url);

				if (serviceName.getValue().equals("OBEX Object Push")) {
					sendMessageToDevice(url);
				}
			} else {
				System.out.println("Service found " + url);
			}
		}
	}

	public void sendMessageToDevice(String serverURL) {
		try {
			System.out.println("Connecting to " + serverURL);

			ClientSession clientSession = (ClientSession) Connector
					.open(serverURL);
			HeaderSet hsConnectReply = clientSession.connect(null);
			if (hsConnectReply.getResponseCode() != ResponseCodes.OBEX_HTTP_OK) {
				System.out.println("Failed to connect");
				return;
			}

			HeaderSet hsOperation = clientSession.createHeaderSet();
			hsOperation.setHeader(HeaderSet.NAME, "Hello.txt");
			hsOperation.setHeader(HeaderSet.TYPE, "text");

			// Create PUT Operation
			Operation putOperation = clientSession.put(hsOperation);

			// Send some text to server
			byte data[] = "Hello World !!!".getBytes("iso-8859-1");
			OutputStream os = putOperation.openOutputStream();
			os.write(data);
			os.close();

			putOperation.close();

			clientSession.disconnect(null);

			clientSession.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public interface IBluetoothDeviceDiscovered {
		public void deviceDiscovered(String name);
	}
}