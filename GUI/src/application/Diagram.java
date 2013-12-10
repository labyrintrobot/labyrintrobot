package application;

import java.util.ArrayList;
import java.util.List;

import javafx.scene.chart.XYChart;

/**
 * A GUI control that displays what the control module is doing to the user.
 * @author Emil Berg
 *
 */
public enum Diagram {
	
	DISTANCE_LEFT_SHORT (0, 40, true, "Distance left, short", "Distance (cm)", "1.2"),
	DISTANCE_LEFT_LONG (0, 256, true, "Distance left, long", "Distance (cm)", "2.1"),
	DISTANCE_FORWARD_LEFT (0, 256, true, "Distance forward, left", "Distance (cm)", "2.2"),
	DISTANCE_FORWARD_CENTER (0, 40, true, "Distance forward, center", "Distance (cm)", "1.1"),
	DISTANCE_FORWARD_RIGHT (0, 256, true, "Distance forward, right", "Distance (cm)", "2.3"),
	DISTANCE_RIGHT_LONG (0, 256, true, "Distance right, long", "Distance (cm)", "2.4"),
	DISTANCE_RIGHT_SHORT (0, 40, true, "Distance right, short", "Distance (cm)", "1.3"),
	CONTROL_ERROR (-128, 128, false, "Control error", "Difference", "Cerr");
	
	private Diagram(int min, int max, boolean unsigned, String title, String yText, String buttonText) {
		this.min = min;
		this.max = max;
		this.unsigned = unsigned;
		this.title = title;
		this.yText = yText;
		this.buttonText = buttonText;
	}
	
	private final List<XYChart.Data<Number, Number>> currentData = new ArrayList<>();
	private List<XYChart.Data<Number, Number>> pausedData = new ArrayList<>();
	private final int min;
	private final int max;
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
	
	public List<XYChart.Data<Number, Number>> getCurrentData() {
		return currentData;
	}
	
	public List<XYChart.Data<Number, Number>> getPausedData() {
		return pausedData;
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

	public int getMin() {
		return min;
	}
	
	public int getMax() {
		return max;
	}

	public boolean isUnsigned() {
		return unsigned;
	}
}
