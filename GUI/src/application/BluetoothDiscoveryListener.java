package application;

import java.io.DataInputStream;
import java.io.IOException;
import java.util.Vector;

import javax.bluetooth.DeviceClass;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.RemoteDevice;
import javax.bluetooth.ServiceRecord;
import javax.bluetooth.UUID;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

public class BluetoothDiscoveryListener implements DiscoveryListener {

	/*private static final boolean DEBUG = true;

	private static final Object lock = new Object();

	private final List<RemoteDevice> devices;*/
	private final IBluetoothDeviceDiscovered deviceDiscoveredCallback;

	public BluetoothDiscoveryListener(IBluetoothDeviceDiscovered deviceDiscoveredCallback) {
		//devices = new ArrayList<>();
		this.deviceDiscoveredCallback = deviceDiscoveredCallback;
	}

	/*public void start() {

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
		if (name.equals("")) {
			name = "-";
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
					//sendMessageToDevice(url);
				}
			} else {
				System.out.println("Service found " + url);
			}
		}
	}*/

	public interface IBluetoothDeviceDiscovered {
		public void deviceDiscovered(String name);
	}
	Vector<RemoteDevice> devices;
	Vector<ServiceRecord> services;
	protected void findDevices() {
		try {
			devices              = new Vector<RemoteDevice>();
			services              = new Vector<ServiceRecord>();
			LocalDevice local    = LocalDevice.getLocalDevice();
			DiscoveryAgent agent = local.getDiscoveryAgent();

			agent.startInquiry(DiscoveryAgent.GIAC, this);
			System.out.println("Starting device discovery...");
		}catch(Exception e) {
			System.out.println("Error initiating discovery.");
		}
	}
	public void deviceDiscovered(RemoteDevice arg0, DeviceClass arg1) {
		try {
			String name = arg0.getFriendlyName(true);

			System.out.println("Found device: " + name);
			deviceDiscoveredCallback.deviceDiscovered(name);
			
			if (name.equals("FireFly-A696")) {
				devices.add(arg0);
			}
		} catch (IOException e) {
			System.out.println("Failed to get remoteDevice Name.");
		}
	}

	public void inquiryCompleted(int arg0) {
		System.out.println("Inquiry Completed.");

		// Start service probing
		for(RemoteDevice d :devices) {
			findServices(d);
		}
	}

	protected void findServices(RemoteDevice device) {
		try {
			UUID[] uuids  = new UUID[1];
			uuids[0]      = new UUID(0x1101);    //The UUID of the each service
			LocalDevice local         = LocalDevice.getLocalDevice();
			DiscoveryAgent agent         = local.getDiscoveryAgent();

			agent.searchServices(null, uuids, device, this);
			System.out.println("Starting Service Discovery...");
		} catch(Exception e){
			System.out.println("Error finding services.");
		}
	}

	public void servicesDiscovered(int arg0, ServiceRecord[] arg1) {
		for(ServiceRecord x : arg1) {
			services.add(x);
		}
	}

	public void serviceSearchCompleted(int arg0, int arg1) {
		System.out.println("Service search completed.");

		broadcastCommand(new String("Hello world!"));
	}

	public void broadcastCommand(String str) {
		System.out.println("Num services: " + services.size());
		for(ServiceRecord sr : services) {
			String url = sr.getConnectionURL(ServiceRecord.AUTHENTICATE_NOENCRYPT, false);

			StreamConnection conn = null;

			try {
				System.out.println("Sending command to " + url);

				conn = (StreamConnection) Connector.open(url);
				DataInputStream din = conn.openDataInputStream();
				
				for (int i = 0; i < 20; i++) {
					System.out.println(din.read());
					System.out.println(din.read());
				}

				din.close();
				conn.close();

				System.out.println("Sent. Connection Closed.");

			} catch (Exception e) {
				System.out.println("Failed to connect to " + url);
				e.printStackTrace();
			}
		}
	}
}