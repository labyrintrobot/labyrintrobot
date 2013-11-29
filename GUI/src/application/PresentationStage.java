package application;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Button;
import javafx.scene.control.Slider;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import application.ChartSelectorPad.SelectedToggleButton;

public class PresentationStage extends Stage implements BluetoothAdapter.IMessageReceiver {
	
	long startTime = System.currentTimeMillis();
	
	private static final String PAUSE_TEXT = "Pause";
	private static final String RESUME_TEXT = "Resume";
	private static final int CHART_BATCHES_MAX = 100;

	private final ControllerAdapter controllerAdapter;
	private final BluetoothAdapter bluetoothAdapter;
	private final ControlPad controlPad;
	private final ChartSelectorPad chartSelectorPad;
	
	private final LineChart<Number, Number> lineChart;
	private final Slider minSlider;
	private final Slider maxSlider;

	private final TextArea errorLog;

	private boolean paused = false;
	
	private int chartBatches = 0;
	
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
	
	private LineChart<Number, Number> generateRealTimeChart(String name, String yAxisText, int maxY) {
		
		// Setup
		NumberAxis xAxis = new NumberAxis(0, 100, 10);
        NumberAxis yAxis = new NumberAxis(0, maxY, maxY/8);
        xAxis.setLabel("Time");
        yAxis.setLabel(yAxisText);
        xAxis.setForceZeroInRange(false);
        
        LineChart<Number, Number> lc = new LineChart<Number, Number>(xAxis, yAxis);
        lc.setAnimated(false);
        lc.setCreateSymbols(false);
        
        lc.setMaxHeight(360);
        
        lc.setTitle(name);
        lc.setLegendVisible(false);
        
        Set<Node> lookupAll = lc.lookupAll(".series0");
        for (Node n : lookupAll) {
        	n.setStyle("-fx-stroke-width: 1px;-fx-stroke: black;");
        }
        
        return lc;
	}
	
	private void setLineChartData(List<TimeValuePair> data, String title) {
        XYChart.Series<Number, Number> series = new XYChart.Series<Number, Number>();
        
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
	
	public PresentationStage(String bluetoothUrl) {

		bluetoothAdapter = new BluetoothAdapter(this);

		lineChart = generateRealTimeChart("Forward left, short", "y", 256);
		
		minSlider = new Slider(0.0, 1.0, 0.0);
		maxSlider = new Slider(0.0, 1.0, 1.0);
		
		controllerAdapter = new ControllerAdapter(bluetoothAdapter);
		controlPad = new ControlPad();
		chartSelectorPad = new ChartSelectorPad(new ChartSelectorPad.ToggleCallback() {
			
			@Override
			public void callback(ChartSelectorPad.SelectedToggleButton stb) {
		        
		        List<TimeValuePair> data;
		        String title = "schlorg";

				switch (stb) {
				case DISTANCE_LEFT_SHORT:
					data = distanceLeftShortList;
					break;
				case DISTANCE_LEFT_LONG:
					data = distanceLeftLongList;
					break;
				case DISTANCE_FORWARD_CENTER:
					data = distanceForwardCenterList;
					break;
				case DISTANCE_FORWARD_LEFT:
					data = distanceForwardLeftList;
					break;
				case DISTANCE_FORWARD_RIGHT:
					data = distanceForwardRightList;
					break;
				case DISTANCE_RIGHT_LONG:
					data = distanceRightLongList;
					break;
				case DISTANCE_RIGHT_SHORT:
					data = distanceRightShortList;
					break;
				case TAPE:
					data = tapeList;
					break;
				case CONTROL_ERROR:
					data = controlErrorList;
					break;
				default:
					// Should not happen
					data = null;
					break;
				}
				
				setLineChartData(data, title);
			}
		});

		errorLog = new TextArea();

		controlPad.pressStop();
		errorLog.setEditable(false);

		try {
			bluetoothAdapter.setup(bluetoothUrl);
			
			this.setTitle("Labyrintrobot");
			final Button pauseButton = new Button();
			if (paused) {
				pauseButton.setText(RESUME_TEXT);
			} else {
				pauseButton.setText(PAUSE_TEXT);
			}

			HBox northBox = new HBox();
			HBox southBox = new HBox();
			VBox centerBox = new VBox();

			northBox.setAlignment(Pos.TOP_CENTER);
			southBox.setAlignment(Pos.BOTTOM_CENTER);
			centerBox.setAlignment(Pos.CENTER);
			
			northBox.getChildren().addAll(minSlider, maxSlider);
			southBox.getChildren().addAll(errorLog, chartSelectorPad);
			centerBox.getChildren().addAll(pauseButton, controlPad, lineChart);

			BorderPane root = new BorderPane();
			root.setTop(northBox);
			root.setBottom(southBox);
			root.setCenter(centerBox);
			Scene scene = new Scene(root, 1280, 1024);
			this.setScene(scene);

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
						case W:
							controllerAdapter.pressW();
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
						case W:
							controllerAdapter.releaseW();
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

			pauseButton.setOnAction(new EventHandler<ActionEvent>() {
				@Override public void handle(ActionEvent e) {
					paused = !paused;
					if (paused) {
						pauseButton.setText(RESUME_TEXT);
					} else {
						pauseButton.setText(PAUSE_TEXT);
					}
				}
			});

			this.setOnCloseRequest(new EventHandler<WindowEvent>() {

				@Override
				public void handle(WindowEvent we) {
					bluetoothAdapter.exit();
				}
			});

			pauseButton.setFocusTraversable(false);

			this.addEventFilter(KeyEvent.KEY_PRESSED, pressEvent);
			this.addEventFilter(KeyEvent.KEY_RELEASED, releaseEvent);

			this.show();

			listen();

		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	private void listen() {
		Thread listenerThread = new Thread(new Runnable() {

			@Override
			public void run() {
				try {
					bluetoothAdapter.receiveMessages();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}

		});
		listenerThread.setDaemon(true);

		listenerThread.start();
	}

	private void log(TextArea ta, String text) {
		ta.setText(ta.getText() + text + '\n');
		ta.positionCaret(ta.getText().length()); // Scroll to end
	}
	
	private void updateLineChartData(List<TimeValuePair> l, int newData, SelectedToggleButton stb, String text) {
		l.add(new TimeValuePair((System.currentTimeMillis() - startTime) / 100, newData));
		if (!paused && chartSelectorPad.getSelected() == stb && chartBatches >= CHART_BATCHES_MAX) {
			chartBatches = 0;
			setLineChartData(l, text);
		}
	}

	@Override
	public void receiveMessage(int header, int data) {
		chartBatches++;
		switch (header) {

		case 0x00:
			log(errorLog, "Unexpected header " + Integer.toHexString(header) + ": 0x" + Integer.toHexString(data));
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
				log(errorLog, "Illegal data received for header 0x" + Integer.toHexString(header) + ": 0x" + Integer.toHexString(data));
				break;
			}
			break;
			
		case 0x02:
			log(errorLog, "Unexpected header " + Integer.toHexString(header) + ": 0x" + Integer.toHexString(data));
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
			log(errorLog, "Error code 0x" + Integer.toHexString(data));
			break;
			
		case 0x0D:
			if (data != 0x00) {
				log(errorLog, "Invalid ping data: 0x" + Integer.toHexString(data));
			}
			// Just respond
			try {
				bluetoothAdapter.sendMessage(header, 0x00);
			} catch (IOException e) {
				e.printStackTrace();
			}
			break;

		default:
			log(errorLog, "Invalid header: 0x" + Integer.toHexString(header) + ", data: 0x" + Integer.toHexString(data));
			break;
		}
	}
}
