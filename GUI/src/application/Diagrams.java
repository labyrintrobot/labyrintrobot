package application;

import java.util.ArrayList;
import java.util.List;

public enum Diagrams {
	
	DISTANCE_LEFT_SHORT (true, "Distance left, short", "Distance (cm)", "1.2"),
	DISTANCE_LEFT_LONG (true, "Distance left, long", "Distance (cm)", "2.1"),
	DISTANCE_FORWARD_LEFT (true, "Distance forward, left", "Distance (cm)", "2.2"),
	DISTANCE_FORWARD_CENTER (true, "Distance forward, center", "Distance (cm)", "1.1"),
	DISTANCE_FORWARD_RIGHT (true, "Distance forward, right", "Distance (cm)", "2.3"),
	DISTANCE_RIGHT_LONG (true, "Distance right, long", "Distance (cm)", "2.4"),
	DISTANCE_RIGHT_SHORT (true, "Distance right, short", "Distance (cm)", "1.3"),
	TAPE (true, "Tape", "Time", "Tape"),
	CONTROL_ERROR (false, "Control error", "Difference", "Cerr");
	
	private Diagrams(boolean unsigned, String title, String yText, String buttonText) {
		this.unsigned = unsigned;
		this.title = title;
		this.yText = yText;
		this.buttonText = buttonText;
	}
	

	public static class TimeValuePair {
		public final long time;
		public final int value;

		public TimeValuePair(long time, int value) {
			this.time = time;
			this.value = value;
		}
	}
	
	private final List<TimeValuePair> currentData = new ArrayList<>();
	private List<TimeValuePair> pausedData = new ArrayList<>();
	private final boolean unsigned;
	private final String title;
	private final String yText;
	private final String buttonText;
	
	public void clearData() {
		this.currentData.clear();
	}
	
	public void savePausedData() {
		this.pausedData = new ArrayList<>(this.currentData);
	}
	
	public List<TimeValuePair> getCurrentData() {
		return currentData;
	}
	
	public List<TimeValuePair> getPausedData() {
		return pausedData;
	}
	
	public boolean isUnsigned() {
		return unsigned;
	}
	
	public String getTitle() {
		return title;
	}
	
	public String getyText() {
		return yText;
	}

	public String getButtonText() {
		return buttonText;
	}
}
