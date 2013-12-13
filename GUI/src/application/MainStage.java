package application;

import indicators.ArmSignalIndicator;
import indicators.ControlSignalIndicator;
import indicators.TapeSignalIndicator;

import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Set;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.chart.XYChart.Data;
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
import application.NumericUpDown.ICallback;

/**
 * The main stage shown. Currently too big for its own good.
 * @author Emil Berg
 *
 */
public class MainStage extends Application implements BluetoothAdapter.IMessageReceiver {

	long startTime = System.currentTimeMillis();

	// Maximum data amount to display on a LineChart
	private static final int MAX_CHART_SIZE = 1200;

	// Bluetooth string to connect to "our" FireFly
	private static final String BLUETOOTH_URL = "btspp://00066603A696:1;authenticate=true;encrypt=false;master=false";
	
	private static final DateFormat hourMinuteSecond = new SimpleDateFormat("HH:mm:ss");

	// Button texts
	private static final String PAUSE_TEXT = "Pause";
	private static final String RESUME_TEXT = "Resume";

	// Adapters and custom controls
	private final ControllerAdapter controllerAdapter;
	private final BluetoothAdapter bluetoothAdapter;
	private final ArmSignalIndicator armSingnalIndicator;
	private final ControlSignalIndicator controlSignalIndicator;
	private final TapeSignalIndicator tapeSignalIndicator;
	private final ChartSelectorControl chartSelectorPad;

	// Buttons
	private final Button pauseButton;
	private final Button clearButton;

	// Other controls
	private final LineChart<Number, Number> lineChart;
	private final Slider minSlider;
	private final Slider maxSlider;
	private final Label minSliderLabel;
	private final Label maxSliderLabel;
	private final ProgressIndicator progressIndicator;

	// Displays errors and debug messages
	private final TextArea errorLog;

	// true if the paused button is in paused mode
	private boolean paused = false;

	// Selecting P in PD regulation
	private final NumericUpDown pSelector;
	// Selecting D in PD regulation
	private final NumericUpDown dSelector;
	// Selecting robot speed
	private final NumericUpDown speedSelector;

	public static void main(String[] args) {
		launch(args);
	}

	public MainStage() {

		this.bluetoothAdapter = new BluetoothAdapter(BLUETOOTH_URL, this);

		this.pauseButton = new Button(PAUSE_TEXT);
		this.clearButton = new Button("Clear");

		this.lineChart = generateRealTimeChart(Diagram.DISTANCE_FORWARD_CENTER);

		this.minSlider = new Slider(0.0, 1.0, 0.0);
		this.maxSlider = new Slider(0.0, 1.0, 1.0);
		this.minSliderLabel = new Label("Min");
		this.maxSliderLabel = new Label("Max");

		this.progressIndicator = new ProgressIndicator(-1.0);

		this.controllerAdapter = new ControllerAdapter(bluetoothAdapter);
		this.armSingnalIndicator = new ArmSignalIndicator();
		this.controlSignalIndicator = new ControlSignalIndicator();
		this.tapeSignalIndicator = new TapeSignalIndicator();
		this.chartSelectorPad = new ChartSelectorControl(new ChartSelectorControl.ToggleCallback() {

			@Override
			public void callback() {
				updateLineChart();
			}
		});
		
		final int MAX = 255;

		this.pSelector = new NumericUpDown("P:", new ICallback() {

			@Override
			public void valueChanged(int newValue) {
				//MainStage.this.bluetoothAdapter.sendMessage(Header.P_LSB, newValue);
			}
		}, 15, 0, MAX);

		this.dSelector = new NumericUpDown("D:", new ICallback() {

			@Override
			public void valueChanged(int newValue) {
				//MainStage.this.bluetoothAdapter.sendMessage(Header.D_LSB, newValue);
			}
		}, 20, 0, MAX);

		this.speedSelector = new NumericUpDown("Speed:", new ICallback() {

			@Override
			public void valueChanged(int newValue) {
				//MainStage.this.bluetoothAdapter.sendMessage(Header.SPEED, newValue);
			}
		}, 128, 0, MAX);

		this.errorLog = new TextArea();
	}

	/**
	 * Generates a LineChart with given properties.
	 */
	private LineChart<Number, Number> generateRealTimeChart(Diagram dia) {

		// Setup
		NumberAxis xAxis = new NumberAxis(0, 100, 100);
		NumberAxis yAxis = new NumberAxis(dia.getMin(), dia.getMax(), (dia.getMax() - dia.getMin()) / 8);
		xAxis.setLabel("Time");
		yAxis.setLabel(dia.getyText());
		xAxis.setForceZeroInRange(false);

		LineChart<Number, Number> lc = new LineChart<>(xAxis, yAxis);
		lc.getData().add(new XYChart.Series<Number, Number>());
		lc.setAnimated(false);
		lc.setCreateSymbols(false);

		lc.setTitle(dia.getTitle());
		lc.setLegendVisible(false);

		Set<Node> lookupAll = lc.lookupAll(".series0");
		for (Node n : lookupAll) {
			n.setStyle("-fx-stroke-width: 1px;-fx-stroke: black;");
		}

		return lc;
	}

	/**
	 * Changes LineChart data completely.
	 */
	private void setLineChartData(final Diagram dia, final boolean paused) {
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				
				List<XYChart.Data<Number, Number>> data;
				if (paused) {
					data = dia.getPausedData();
				} else {
					data = dia.getPausedData();
				}
				
				XYChart.Series<Number, Number> series = new XYChart.Series<>();

				lineChart.setTitle(dia.getTitle());

				int lower = (int) (minSlider.getValue() * data.size());
				int upper = (int) (maxSlider.getValue() * data.size());

				series.getData().addAll(data.subList(lower, upper));

				NumberAxis xAxis = (NumberAxis) lineChart.getXAxis();
				NumberAxis yAxis = (NumberAxis) lineChart.getYAxis();

				if (data.size() != 0) {
					xAxis.setLowerBound(data.get(lower).getXValue().doubleValue());
					xAxis.setUpperBound(data.get(Math.max(upper - 1, 0)).getXValue().doubleValue());
				}

				yAxis.setLabel(dia.getyText());
				yAxis.setLowerBound(dia.getMin());
				yAxis.setUpperBound(dia.getMax());
				yAxis.setTickUnit((dia.getMax() - dia.getMin()) / 8);

				lineChart.getData().clear();
				lineChart.getData().add(series);

				Set<Node> lookupAll = lineChart.lookupAll(".series0");
				for (Node n : lookupAll) {
					n.setStyle("-fx-stroke-width: 1px;-fx-stroke: black;");
				}
			}
		});
	}

	/**
	 * Adds LineChart data, for effective data insertion.
	 * @param tvp
	 */
	private void addLineChartData(final XYChart.Data<Number, Number> newData) {
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				XYChart.Series<Number, Number> series = lineChart.getData().get(0);

				ObservableList<Data<Number, Number>> data = series.getData();
				data.add(newData);

				if (data.size() > MAX_CHART_SIZE) {
					data.subList(0, data.size() - MAX_CHART_SIZE).clear();
				}

				int lower = (int) (minSlider.getValue() * data.size());
				int upper = (int) (maxSlider.getValue() * data.size());

				NumberAxis xAxis = (NumberAxis) lineChart.getXAxis();

				if (data.size() != 0) {
					xAxis.setLowerBound(data.get(lower).getXValue().longValue());
					xAxis.setUpperBound(data.get(Math.max(upper - 1, 0)).getXValue().longValue());
				}
			}
		});
	}

	/**
	 * Updates the data for the currently selected LineChart.
	 */
	private void updateLineChart() {
		Diagram dia = chartSelectorPad.getSelected();
		setLineChartData(dia, this.paused);
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
		
		this.errorLog.setEditable(false);

		primaryStage.setTitle("Labyrintrobot");
		if (paused) {
			this.pauseButton.setText(RESUME_TEXT);
		} else {
			this.pauseButton.setText(PAUSE_TEXT);
		}

		VBox northBox = new VBox();
		HBox southBox = new HBox();
		HBox buttonBox = new HBox();
		VBox southChildBox1 = new VBox();
		VBox southChildBox2 = new VBox();
		VBox southChildBox3 = new VBox();

		northBox.setAlignment(Pos.TOP_CENTER);
		southBox.setAlignment(Pos.BOTTOM_CENTER);
		buttonBox.setAlignment(Pos.CENTER);
		southChildBox1.setAlignment(Pos.CENTER);
		southChildBox2.setAlignment(Pos.CENTER);
		southChildBox3.setAlignment(Pos.CENTER);

		southChildBox1.getChildren().addAll(this.armSingnalIndicator, this.controlSignalIndicator);
		southChildBox2.getChildren().addAll(this.chartSelectorPad, this.tapeSignalIndicator);
		southChildBox3.getChildren().addAll(this.pSelector, this.dSelector, this.speedSelector);
		buttonBox.getChildren().addAll(this.pauseButton,this.progressIndicator, this.clearButton);
		northBox.getChildren().addAll(buttonBox, this.minSlider, this.minSliderLabel, this.maxSlider, this.maxSliderLabel);
		southBox.getChildren().addAll(this.errorLog, southChildBox1, southChildBox2, southChildBox3);
		
		buttonBox.setSpacing(16);
		southBox.setSpacing(16);
		northBox.setPadding(new Insets(16));
		southChildBox1.setSpacing(16);
		southChildBox2.setSpacing(16);
		southChildBox3.setSpacing(16);

		BorderPane root = new BorderPane();
		root.setTop(northBox);
		root.setBottom(southBox);
		root.setCenter(this.lineChart);
		Scene scene = new Scene(root, 1280, 720);
		primaryStage.setScene(scene);

		EventHandler<KeyEvent> pressEvent = new EventHandler<KeyEvent>() {

			@Override
			public void handle(KeyEvent t) {
				try {
					switch (t.getCode()) {
					// Controller press
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
					case W:
						controllerAdapter.pressW();
						break;
					case E:
						controllerAdapter.pressE();
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
					// Controller release
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
					case W:
						controllerAdapter.releaseW();
						break;
					case E:
						controllerAdapter.releaseE();
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
					for (Diagram s : Diagram.values()) {
						s.savePausedData();
					}
				} else {
					pauseButton.setText(PAUSE_TEXT);
					maxSlider.setValue(1.0);

					updateLineChart();
				}
			}
		});

		// Clear the LineChart and log
		this.clearButton.setOnAction(new EventHandler<ActionEvent>() {
			@Override public void handle(ActionEvent e) {
				for (Diagram s : Diagram.values()) {
					s.clearData();
				}

				startTime = System.currentTimeMillis();
				updateLineChart();
				MainStage.this.errorLog.setText("");
				
				armSingnalIndicator.clear();
				controlSignalIndicator.clear();
				tapeSignalIndicator.clear();
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

		Thread st = new Thread(new SendThread(this.bluetoothAdapter, pSelector, dSelector, speedSelector));
		st.setDaemon(true);
		st.start();
		
		listen();
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

	/**
	 * Logs in "terminal"
	 */
	private void log(final String text) {
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				final StringBuilder sb = new StringBuilder();
				sb.append(errorLog.getText());
				sb.append(hourMinuteSecond.format(new Date()));
				sb.append(": ");
				sb.append(text);
				sb.append('\n');
				MainStage.this.errorLog.setText(sb.toString());
				MainStage.this.errorLog.positionCaret(MainStage.this.errorLog.getText().length()); // Scroll to end
			}
		});
	}

	/**
	 * Helper function for receiveMessage.
	 */
	private void updateLineChartData(int newData, Diagram dia) {
		if (! dia.isUnsigned()) {
			newData = (byte) newData;
		}
		XYChart.Data<Number, Number> data = new XYChart.Data<Number, Number>((System.currentTimeMillis() - startTime) / 100, newData);
		dia.getCurrentData().add(data);
		if (! paused) {
			if (chartSelectorPad.getSelected() == dia) {
				if (dia.getCurrentData().size() > MAX_CHART_SIZE) {
					dia.getCurrentData().subList(0, dia.getCurrentData().size() - MAX_CHART_SIZE).clear();
				}
				addLineChartData(data);
			}
		}
	}

	/**
	 * Callback function from BluetoothAdapter
	 */
	@Override
	public void receiveMessage(Header header, int data) {
		switch (header) {
		case CONTROL_SIGNAL:
			switch (data) {

			case 0x00:
				this.controlSignalIndicator.pressForward();
				log("Signal - Forward");
				break;

			case 0x01:
				this.controlSignalIndicator.pressBackwards();
				log("Signal - Backwards");
				break;

			case 0x02:
				this.controlSignalIndicator.pressRightForward();
				log("Signal - Turning right");
				break;

			case 0x03:
				this.controlSignalIndicator.pressLeftForward();
				log("Signal - Turning left 90 deg");
				break;

			case 0x04:
				this.controlSignalIndicator.pressRotateRight();
				log("Signal - Rotating right 90 deg");
				break;

			case 0x05:
				this.controlSignalIndicator.pressRotateLeft();
				log("Signal - Rotating left");
				break;

			case 0x06:
				this.controlSignalIndicator.pressStop();
				log("Signal - Stop");
				break;
				
			case 0x07:
				this.controlSignalIndicator.pressRotateRight();
				log("Signal - Rotate right");
				break;

			case 0x08:
				this.controlSignalIndicator.pressRotateLeft();
				log("Signal - Rotate left");
				break;
				
			case 0x09:
				this.armSingnalIndicator.pressOpenArm();
				log("Signal - Open arm");
				break;

			case 0x0A:
				this.armSingnalIndicator.pressCloseArm();
				log("Signal - Close arm");
				break;

			default:
				log("Illegal header or data received for " + header.name() + ": 0x" + Integer.toHexString(data));
				break;
			}
			break;
		case DISTANCE_LEFT_SHORT:
			updateLineChartData(data, Diagram.DISTANCE_LEFT_SHORT);
			break;
		case DISTANCE_LEFT_LONG:
			updateLineChartData(data, Diagram.DISTANCE_LEFT_LONG);
			break;
		case DISTANCE_FORWARD_LEFT:
			updateLineChartData(data, Diagram.DISTANCE_FORWARD_LEFT);
			break;
		case DISTANCE_FORWARD_CENTER:
			updateLineChartData(data, Diagram.DISTANCE_FORWARD_CENTER);
			break;
		case DISTANCE_FORWARD_RIGHT:
			updateLineChartData(data, Diagram.DISTANCE_FORWARD_RIGHT);
			break;
		case DISTANCE_RIGHT_LONG:
			updateLineChartData(data, Diagram.DISTANCE_RIGHT_LONG);
			break;
		case DISTANCE_RIGHT_SHORT:
			updateLineChartData(data, Diagram.DISTANCE_RIGHT_SHORT);
			break;
		case CONTROL_ERROR:
			updateLineChartData(data, Diagram.CONTROL_ERROR);
			break;
		case TAPE:
			switch (data) {
			
			case 0x00:
				this.tapeSignalIndicator.pressTapeRight();
				log("Tape signal - Right");
				break;
				
			case 0x01:
				this.tapeSignalIndicator.pressTapeLeft();
				log("Tape signal - Left");
				break;
				
			case 0x02:
				this.tapeSignalIndicator.pressTapeStart();
				log("Tape signal - Start");
				break;
				
			case 0x03:
				this.tapeSignalIndicator.pressTapeFinish();
				log("Tape signal - Finish");
				break;
				
			default:
				log("Illegal tape data received: 0x" + Integer.toHexString(data));
				break;
			}
			break;
		case ERROR:
			log("Error: 0x" + Integer.toHexString(data));
			break;
		case PING:
			if (data != 0x00) {
				log("Invalid ping data: 0x" + Integer.toHexString(data));
			}
			// Ping. Just respond.
			bluetoothAdapter.sendMessage(header, 0x00);
			break;
		default:
			log("Unexpected but valid header received: " + header.name());
			break;
		}
	}

	/**
	 * Callback from BluetoothAdapter.
	 */
	@Override
	public void communicationGained() {
		// Finished
		Platform.runLater(new Runnable() {

			@Override
			public void run() {
				// Just to see when there is a Bluetooth connection
				MainStage.this.progressIndicator.setProgress(1.0);
			}
		});
	}

	/**
	 * Callback from BluetoothAdapter.
	 */
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

	@Override
	public void receiveInvalidMessage(int header, int data) {
		log("Invalid header 0x" + Integer.toHexString(header) + ": 0x" + Integer.toHexString(data));
	}
}
