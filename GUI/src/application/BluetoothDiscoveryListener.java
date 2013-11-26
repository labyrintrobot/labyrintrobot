package application;

import java.io.IOException;

import javax.bluetooth.DeviceClass;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.RemoteDevice;
import javax.bluetooth.ServiceRecord;
import javax.bluetooth.UUID;

public class BluetoothDiscoveryListener implements DiscoveryListener {

	// http://www.almightybuserror.com/2011/09/06/android-to-pc-bluetooth-communication.html

	private static final boolean DEBUG = true;
	
	private final String deviceStr;
	private RemoteDevice device = null;
	private ServiceRecord service = null;
	private final IServicesDiscoveredCallback callback;

	public BluetoothDiscoveryListener(String deviceStr, IServicesDiscoveredCallback callback) {
		this.deviceStr = deviceStr;
		this.callback = callback;
	}
	
	public interface IServicesDiscoveredCallback {
		public void servicesDiscovered(ServiceRecord service);
	}

	public void findDevices() {
		try {
			LocalDevice local = LocalDevice.getLocalDevice();
			DiscoveryAgent agent = local.getDiscoveryAgent();

			agent.startInquiry(DiscoveryAgent.GIAC, this);
			debug("Starting device discovery...");
		}catch(Exception e) {
			debug("Error initiating discovery.");
		}
	}

	@Override
	public void deviceDiscovered(RemoteDevice arg0, DeviceClass arg1) {
		try {
			String name = arg0.getFriendlyName(true);

			debug("Found device: " + name);
			if (name.equals(this.deviceStr)) {
				device = arg0;
			}
		} catch (IOException e) {
			debug("Failed to get remoteDevice Name.");
		}
	}

	@Override
	public void inquiryCompleted(int arg0) {
		debug("Inquiry Completed.");

		try {
			UUID[] uuids  = new UUID[1];
			uuids[0] = new UUID(0x1101);    //The UUID of the each service
			LocalDevice local = LocalDevice.getLocalDevice();
			DiscoveryAgent agent = local.getDiscoveryAgent();

			agent.searchServices(null, uuids, device, this);
			debug("Starting Service Discovery...");
		} catch(Exception e){
			debug("Error finding services.");
		}
	}

	@Override
	public void servicesDiscovered(int arg0, ServiceRecord[] arg1) {
		if (arg1.length == 0) {
			throw new IllegalStateException();
		}
		this.service = arg1[0];
	}

	@Override
	public void serviceSearchCompleted(int arg0, int arg1) {
		debug("Service search completed.");
		
		this.callback.servicesDiscovered(this.service);
	}
	
	private void debug(String str) {
		if (DEBUG) {
			System.out.println(str);
		}
	}
}