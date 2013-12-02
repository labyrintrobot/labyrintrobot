package application;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.control.Slider;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import application.ChartSelectorPad.SelectedToggleButton;

public class MainStage extends Application implements BluetoothAdapter.IMessageReceiver {

	long startTime = System.currentTimeMillis();

	private final String BLUETOOTH_URL = "btspp://00066603A696:1;authenticate=true;encrypt=false;master=false";
	private static final String PAUSE_TEXT = "Pause";
	private static final String RESUME_TEXT = "Resume";
	private final ControllerAdapter controllerAdapter;
	private final BluetoothAdapter bluetoothAdapter;
	private final ControlPad controlPad;
	private final ChartSelectorPad chartSelectorPad;

	private final Button pauseButton;
	private final Button clearButton;
	
	private final LineChart<Number, Number> lineChart;
	private final Slider minSlider;
	private final Slider maxSlider;
	private final Label minSliderLabel;
	private final Label maxSliderLabel;
	private final ProgressIndicator progressIndicator;

	private final TextArea errorLog;

	private boolean paused = false;

	private static class TimeValuePair {
		public final long time;
		public final int value;

		public TimeValuePair(long time, int value) {
			this.time = time;
			this.value = value;
		}
	}

	private final List<TimeValuePair> distanceLeftShortList = new ArrayList<>();
	private final List<TimeValuePair> distanceLeftLongList = new ArrayList<>();
	private final List<TimeValuePair> distanceForwardLeftList = new ArrayList<>();
	private final List<TimeValuePair> distanceForwardCenterList = new ArrayList<>();
	private final List<TimeValuePair> distanceForwardRightList = new ArrayList<>();
	private final List<TimeValuePair> distanceRightLongList = new ArrayList<>();
	private final List<TimeValuePair> distanceRightShortList = new ArrayList<>();
	private final List<TimeValuePair> tapeList = new ArrayList<>();
	private final List<TimeValuePair> controlErrorList = new ArrayList<>();

	public static void main(String[] args) {
		launch(args);
	}

	public MainStage() {

		this.bluetoothAdapter = new BluetoothAdapter(BLUETOOTH_URL, this);

		this.pauseButton = new Button(PAUSE_TEXT);
		this.clearButton = new Button("Clear");
		
		this.lineChart = generateRealTimeChart("Forward left, short", "y", 256);

		this.minSlider = new Slider(0.0, 1.0, 0.0);
		this.maxSlider = new Slider(0.0, 1.0, 1.0);
		this.minSliderLabel = new Label("Min");
		this.maxSliderLabel = new Label("Max");

		this.progressIndicator = new ProgressIndicator(-1.0);

		this.controllerAdapter = new ControllerAdapter(bluetoothAdapter);
		this.controlPad = new ControlPad();
		this.chartSelectorPad = new ChartSelectorPad(new ChartSelectorPad.ToggleCallback() {

			@Override
			public void callback(ChartSelectorPad.SelectedToggleButton stb) {
				GetFromSelRetPair gfsrp = getFromSel(stb);
				setLineChartData(gfsrp.l, gfsrp.s);
			}
		});

		this.errorLog = new TextArea();
	}

	private LineChart<Number, Number> generateRealTimeChart(String name, String yAxisText, int maxY) {
		
		// Setup
		NumberAxis xAxis = new NumberAxis(0, 100, 10);
		NumberAxis yAxis = new NumberAxis(0, maxY, maxY/8);
		xAxis.setLabel("Time");
		yAxis.setLabel(yAxisText);
		xAxis.setForceZeroInRange(false);

		LineChart<Number, Number> lc = new LineChart<>(xAxis, yAxis);
		lc.getData().add(new XYChart.Series<Number, Number>());
		lc.setAnimated(false);
		lc.setCreateSymbols(false);

		lc.setTitle(name);
		lc.setLegendVisible(false);

		Set<Node> lookupAll = lc.lookupAll(".series0");
		for (Node n : lookupAll) {
			n.setStyle("-fx-stroke-width: 1px;-fx-stroke: black;");
		}

		return lc;
	}

	private void setLineChartData(final List<TimeValuePair> data, final String title) {
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				XYChart.Series<Number, Number> series = new XYChart.Series<>();

				lineChart.setTitle(title);

				int lower = (int) (minSlider.getValue() * data.size());
				int upper = (int) (maxSlider.getValue() * data.size());

				for (int i = lower; i < upper; i++) {
					TimeValuePair tvp = data.get(i);
					series.getData().add(new XYChart.Data<Number, Number>(tvp.time, tvp.value));
				}

				NumberAxis xAxis = (NumberAxis) lineChart.getXAxis();

				if (data.size() != 0) {
					xAxis.setLowerBound(data.get(lower).time);
					xAxis.setUpperBound(data.get(upper - 1).time);
				}

				lineChart.getData().remove(0, lineChart.getData().size());
				lineChart.getData().add(series);

				Set<Node> lookupAll = lineChart.lookupAll(".series0");
				for (Node n : lookupAll) {
					n.setStyle("-fx-stroke-width: 1px;-fx-stroke: black;");
				}
			}
		});
	}
	
	private void addLineChartData(final TimeValuePair data) {
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				XYChart.Series<Number, Number> series = lineChart.getData().get(0);

				series.getData().add(new XYChart.Data<Number, Number>(data.time, data.value));
				
				int lower = (int) (minSlider.getValue() * series.getData().size());
				int upper = (int) (maxSlider.getValue() * series.getData().size());
				
				NumberAxis xAxis = (NumberAxis) lineChart.getXAxis();

				if (series.getData().size() != 0) {
					xAxis.setLowerBound(series.getData().get(lower).getXValue().longValue());
					xAxis.setUpperBound(series.getData().get(upper - 1).getXValue().longValue());
				}
			}
		});
	}
	
	private void updateLineChart() {
		GetFromSelRetPair gfsrp = getFromSel(chartSelectorPad.getSelected());
		setLineChartData(gfsrp.l, gfsrp.s);
	}

	public void start(Stage primaryStage) {

		this.minSlider.valueProperty().addListener(new ChangeListener<Number>() {

			@Override
			public void changed(ObservableValue<? extends Number> arg0,
					Number arg1, Number arg2) {

				minSlider.setValue(Math.min(minSlider.getValue(), maxSlider.getValue()));

				if (paused) {
					updateLineChart();
				}
			}
		});

		this.maxSlider.valueProperty().addListener(new ChangeListener<Number>() {

			@Override
			public void changed(ObservableValue<? extends Number> arg0,
					Number arg1, Number arg2) {

				maxSlider.setValue(Math.max(minSlider.getValue(), maxSlider.getValue()));

				if (paused) {
					updateLineChart();
				}
			}
		});

		this.controlPad.pressStop();
		this.errorLog.setEditable(false);

		primaryStage.setTitle("Labyrintrobot");
		if (paused) {
			this.pauseButton.setText(RESUME_TEXT);
		} else {
			this.pauseButton.setText(PAUSE_TEXT);
		}
		
		VBox northBox = new VBox();
		HBox southBox = new HBox();
		VBox centerBox = new VBox();
		
		northBox.setAlignment(Pos.TOP_CENTER);
		southBox.setAlignment(Pos.BOTTOM_CENTER);
		centerBox.setAlignment(Pos.CENTER);
		
		northBox.getChildren().addAll(minSlider, minSliderLabel, maxSliderLabel, maxSlider);
		centerBox.getChildren().addAll(progressIndicator, pauseButton, clearButton, lineChart);
		southBox.getChildren().addAll(errorLog, controlPad, chartSelectorPad);
		
		BorderPane root = new BorderPane();
		root.setTop(northBox);
		root.setBottom(southBox);
		root.setCenter(centerBox);
		Scene scene = new Scene(root, 1280, 1024);
		primaryStage.setScene(scene);
		
		EventHandler<KeyEvent> pressEvent = new EventHandler<KeyEvent>() {
			
			@Override
			public void handle(KeyEvent t) {
				try {
					switch (t.getCode()) {
					case RIGHT:
						controllerAdapter.pressRight();
						break;
					case UP:
						controllerAdapter.pressUp();
						break;
					case LEFT:
						controllerAdapter.pressLeft();
						break;
					case DOWN:
						controllerAdapter.pressDown();
						break;
					case C:
						controllerAdapter.pressC();
						break;
					case Q:
						controllerAdapter.pressQ();
						break;
					default:
						// Do nothing
						break;
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		};
		
		EventHandler<KeyEvent> releaseEvent = new EventHandler<KeyEvent>() {
			
			@Override
			public void handle(KeyEvent t) {
				try {
					switch (t.getCode()) {
					case RIGHT:
						controllerAdapter.releaseRight();
						break;
					case UP:
						controllerAdapter.releaseUp();
						break;
					case LEFT:
						controllerAdapter.releaseLeft();
						break;
					case DOWN:
						controllerAdapter.releaseDown();
						break;
					case C:
						controllerAdapter.releaseC();
						break;
					case Q:
						controllerAdapter.releaseQ();
						break;
					default:
						// Do nothing
						break;
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		};
		
		this.pauseButton.setOnAction(new EventHandler<ActionEvent>() {
			@Override public void handle(ActionEvent e) {
				paused = !paused;
				if (paused) {
					pauseButton.setText(RESUME_TEXT);
				} else {
					pauseButton.setText(PAUSE_TEXT);
					maxSlider.setValue(1.0);
				}
			}
		});
		
		this.clearButton.setOnAction(new EventHandler<ActionEvent>() {
			@Override public void handle(ActionEvent e) {
				distanceLeftShortList.clear();
				distanceLeftLongList.clear();
				distanceForwardLeftList.clear();
				distanceForwardCenterList.clear();
				distanceForwardRightList.clear();
				distanceRightLongList.clear();
				distanceRightShortList.clear();
				tapeList.clear();
				controlErrorList.clear();
				
				startTime = System.currentTimeMillis();
				updateLineChart();
				MainStage.this.errorLog.setText("");
			}
		});
		
		primaryStage.setOnCloseRequest(new EventHandler<WindowEvent>() {
			
			@Override
			public void handle(WindowEvent we) {
				bluetoothAdapter.exit();
			}
		});
		
		primaryStage.addEventFilter(KeyEvent.KEY_PRESSED, pressEvent);
		primaryStage.addEventFilter(KeyEvent.KEY_RELEASED, releaseEvent);
		
		primaryStage.show();
		
		listen();
	}

	private static class GetFromSelRetPair {
		public String s;
		public List<TimeValuePair> l;
	}

	private GetFromSelRetPair getFromSel(ChartSelectorPad.SelectedToggleButton stb) {

		GetFromSelRetPair gfsrp = new GetFromSelRetPair();

		switch (stb) {
		case DISTANCE_LEFT_SHORT:
			gfsrp.s = "Distance left, short";
			gfsrp.l = distanceLeftShortList;
			break;
		case DISTANCE_LEFT_LONG:
			gfsrp.s = "Distance left, long";
			gfsrp.l = distanceLeftLongList;
			break;
		case DISTANCE_FORWARD_CENTER:
			gfsrp.s = "Distance forward, center";
			gfsrp.l = distanceForwardCenterList;
			break;
		case DISTANCE_FORWARD_LEFT:
			gfsrp.s = "Distance forward, left";
			gfsrp.l = distanceForwardLeftList;
			break;
		case DISTANCE_FORWARD_RIGHT:
			gfsrp.s = "Distance forward, right";
			gfsrp.l = distanceForwardRightList;
			break;
		case DISTANCE_RIGHT_LONG:
			gfsrp.s = "Distance right, long";
			gfsrp.l = distanceRightLongList;
			break;
		case DISTANCE_RIGHT_SHORT:
			gfsrp.s = "Distance right, short";
			gfsrp.l = distanceRightShortList;
			break;
		case TAPE:
			gfsrp.s = "Tape width";
			gfsrp.l = tapeList;
			break;
		case CONTROL_ERROR:
			gfsrp.s = "Control error";
			gfsrp.l = controlErrorList;
			break;
		default:
			// Should not happen
			return null;
		}

		return gfsrp;
	}

	private void listen() {
		Thread listenerThread = new Thread(new Runnable() {

			@Override
			public void run() {
				bluetoothAdapter.receiveMessages();
			}

		});
		listenerThread.setDaemon(true);

		listenerThread.start();
	}

	private void log(final String text) {
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				MainStage.this.errorLog.setText(MainStage.this.errorLog.getText() + text + '\n');
				MainStage.this.errorLog.positionCaret(MainStage.this.errorLog.getText().length()); // Scroll to end
			}
		});
	}

	private void updateLineChartData(List<TimeValuePair> l, int newData, SelectedToggleButton stb, String text) {
		TimeValuePair tvp = new TimeValuePair((System.currentTimeMillis() - startTime) / 100, newData);
		l.add(tvp);
		if (!paused && chartSelectorPad.getSelected() == stb) {
			addLineChartData(tvp);
		}
	}

	@Override
	public void receiveMessage(int header, int data) {
		switch (header) {

		case 0x00:
			log("Unexpected header " + Integer.toHexString(header) + ": 0x" + Integer.toHexString(data));
			break;

		case 0x01:
			switch (data) {

			case 0x00:
				controlPad.pressForward();
				break;

			case 0x01:
				controlPad.pressBackwards();
				break;

			case 0x02:
				controlPad.pressRightForward();
				break;

			case 0x03:
				controlPad.pressLeftForward();
				break;

			case 0x04:
				controlPad.pressRotateRight();
				break;

			case 0x05:
				controlPad.pressRotateLeft();
				break;

			case 0x06:
				controlPad.pressStop();
				break;

			default:
				log("Illegal data received for header 0x" + Integer.toHexString(header) + ": 0x" + Integer.toHexString(data));
				break;
			}
			break;

		case 0x02:
			log("Unexpected header " + Integer.toHexString(header) + ": 0x" + Integer.toHexString(data));
			break;

		case 0x03:
			updateLineChartData(distanceLeftShortList, data, SelectedToggleButton.DISTANCE_LEFT_SHORT, "Distance left, short");
			break;

		case 0x04:
			updateLineChartData(distanceLeftLongList, data, SelectedToggleButton.DISTANCE_LEFT_LONG, "Distance left, long");
			break;

		case 0x05:
			updateLineChartData(distanceForwardLeftList, data, SelectedToggleButton.DISTANCE_FORWARD_LEFT, "Distance forward left");
			break;

		case 0x06:
			updateLineChartData(distanceForwardCenterList, data, SelectedToggleButton.DISTANCE_FORWARD_CENTER, "Distance forward center");
			break;

		case 0x07:
			updateLineChartData(distanceForwardRightList, data, SelectedToggleButton.DISTANCE_FORWARD_RIGHT, "Distance forward right");
			break;

		case 0x08:
			updateLineChartData(distanceRightLongList, data, SelectedToggleButton.DISTANCE_RIGHT_LONG, "Distance right long");
			break;

		case 0x09:
			updateLineChartData(distanceRightShortList, data, SelectedToggleButton.DISTANCE_RIGHT_SHORT, "Distance right short");
			break;

		case 0x0A:
			updateLineChartData(controlErrorList, data, SelectedToggleButton.CONTROL_ERROR, "Control error");
			break;

		case 0x0B:
			updateLineChartData(tapeList, data, SelectedToggleButton.TAPE, "Tape width");
			break;

		case 0x0C:
			log("Error code 0x" + Integer.toHexString(data));
			break;

		case 0x0D:
			if (data != 0x00) {
				log("Invalid ping data: 0x" + Integer.toHexString(data));
			}
			// Ping. Just respond.
			bluetoothAdapter.sendMessage(header, 0x00);
			break;

		default:
			log("Invalid header: 0x" + Integer.toHexString(header) + ", data: 0x" + Integer.toHexString(data));
			break;
		}
	}

	@Override
	public void communicationGained() {
		// Finished
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				MainStage.this.progressIndicator.setProgress(1.0);
			}
		});
	}

	@Override
	public void communicationLost() {
		// Keep "working"
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				MainStage.this.progressIndicator.setProgress(-1.0);
			}
		});
	}
}
