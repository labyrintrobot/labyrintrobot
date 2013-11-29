package application;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

public class PresentationStage extends Stage implements BluetoothAdapter.IMessageReceiver {

	private static final boolean NEW_TEST = true;
	
	private static final String PAUSE_TEXT = "Pause";
	private static final String RESUME_TEXT = "Resume";

	private final ControllerAdapter controllerAdapter;
	private final BluetoothAdapter bluetoothAdapter;
	private final ControlPad controlPad;
	private final ChartSelectorPad chartSelectorPad;

	/*private final RealTimeChart leftChart;
	private final RealTimeChart rightChart;
	private final RealTimeChart forwardLeftChart;
	private final RealTimeChart forwardRightChart;
	private final RealTimeChart forwardChart;
	private final RealTimeChart gyroChart;
	private final RealTimeChart tapeChart;*/
	
	private final LineChart<Number, Number> lineChart;

	private final TextArea errorLog;

	private boolean paused = false;
	
	//////////////////////////////
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
	
	private static final int DATA_MAX = 200;
	
	private int i;
	
	private LineChart<Number, Number> generateRealTimeChart(String name, String yAxisText, int maxY, TimeValuePair[] data) {
		
		// Setup
		NumberAxis xAxis = new NumberAxis(0, DATA_MAX, DATA_MAX / 8);
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
        
        // Add data
        
        XYChart.Series<Number, Number> series = new XYChart.Series<Number, Number>();
        series.setName(name);
        
        lc.getData().add(series);
        ObservableList<XYChart.Data<Number, Number>> lcData = series.getData();
        
        for (TimeValuePair tp : data) {
        	lcData.add(new XYChart.Data<Number, Number>(tp.time, tp.value));
        }
        
        series.setData(lcData);
        
        return lc;
	}
/////////////////////////
	public PresentationStage(String bluetoothUrl) {

		bluetoothAdapter = new BluetoothAdapter(this);

		lineChart = generateRealTimeChart("NAME", "y", 128, new TimeValuePair[0]);

		controllerAdapter = new ControllerAdapter(bluetoothAdapter);
		controlPad = new ControlPad();
		chartSelectorPad = new ChartSelectorPad(new ChartSelectorPad.ToggleCallback() {
			
			@Override
			public void callback(ChartSelectorPad.SelectedToggleButton stb) {
		        XYChart.Series<Number, Number> series = new XYChart.Series<Number, Number>();
		        series.setName("horg");
		        
		        TimeValuePair[] data;

				switch (stb) {
				case DISTANCE_FORWARD_CENTER:
					data = distanceForwardCenterList.toArray(new TimeValuePair[distanceForwardCenterList.size()]);
					break;
				case DISTANCE_FORWARD_LEFT:
					data = distanceForwardLeftList.toArray(new TimeValuePair[distanceForwardLeftList.size()]);
					break;
				case DISTANCE_FORWARD_RIGHT:
					data = distanceForwardRightList.toArray(new TimeValuePair[distanceForwardRightList.size()]);
					break;
				case DISTANCE_LEFT_LONG:
					data = distanceLeftLongList.toArray(new TimeValuePair[distanceLeftLongList.size()]);
					break;
				case DISTANCE_LEFT_SHORT:
					data = distanceLeftShortList.toArray(new TimeValuePair[distanceLeftShortList.size()]);
					break;
				case DISTANCE_RIGHT_LONG:
					data = distanceRightLongList.toArray(new TimeValuePair[distanceRightLongList.size()]);
					break;
				case DISTANCE_RIGHT_SHORT:
					data = distanceRightShortList.toArray(new TimeValuePair[distanceRightShortList.size()]);
					break;
				case TAPE:
					data = tapeList.toArray(new TimeValuePair[tapeList.size()]);
					break;
				default:
					data = null;
					// Should not happen
					break;
				}
				
		        for (TimeValuePair tp : data) {
		        	// TODO: Add all
		        	series.getData().add(new XYChart.Data<Number, Number>(tp.time, tp.value));
		        }
		        
		        NumberAxis xAxis = (NumberAxis) lineChart.getXAxis();
		        
		        xAxis.setLowerBound(data[0].time);
		        xAxis.setUpperBound(data[data.length-1].time);
		        
		        lineChart.getData().remove(0, lineChart.getData().size());
		        lineChart.getData().add(series);
			}
		});

		//rightChart = new RealTimeChart("Right", "Distance", 256);
		//forwardRightChart = new RealTimeChart("Forward, right", "Distance", 256);
		//forwardChart = new RealTimeChart("Forward", "Distance", 256);
		//forwardLeftChart = new RealTimeChart("Forward, left", "Distance", 256);
		//leftChart = new RealTimeChart("Left", "Distance", 256);
		//gyroChart = new RealTimeChart("Gyro", "Angular rate", 256);
		//tapeChart = new RealTimeChart("Tape sensor", "Hamming distance", 8);

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

			VBox eastBox = new VBox();
			VBox westBox = new VBox();
			HBox northBox = new HBox();
			HBox southBox = new HBox();
			VBox centerBox = new VBox();

			eastBox.setAlignment(Pos.CENTER_RIGHT);
			northBox.setAlignment(Pos.TOP_CENTER);
			westBox.setAlignment(Pos.CENTER_RIGHT);
			southBox.setAlignment(Pos.BOTTOM_CENTER);
			centerBox.setAlignment(Pos.CENTER);
			

			/*eastBox.getChildren().add(rightChart.getUnderLyingLineChart());
			westBox.getChildren().add(leftChart.getUnderLyingLineChart());
			northBox.getChildren().addAll(forwardLeftChart.getUnderLyingLineChart(), forwardChart.getUnderLyingLineChart(), forwardRightChart.getUnderLyingLineChart());*/
			southBox.getChildren().addAll(errorLog, /*tapeChart.getUnderLyingLineChart(), gyroChart.getUnderLyingLineChart(),*/ chartSelectorPad);
			centerBox.getChildren().addAll(pauseButton, controlPad, lineChart);

			BorderPane root = new BorderPane();
			root.setRight(eastBox);
			root.setTop(northBox);
			root.setLeft(westBox);
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
						default:
							// Do nothing
							break;
						}
					} catch (IOException e) {
						// TODO Auto-generated catch block
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
						default:
							// Do nothing
							break;
						}
					} catch (IOException e) {
						// TODO Auto-generated catch block
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
					// TODO
					/*rightChart.flush();
					forwardRightChart.flush();
					forwardChart.flush();
					forwardLeftChart.flush();
					leftChart.flush();
					gyroChart.flush();
					tapeChart.flush();*/
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

	@Override
	public void receiveMessage(int header, int data) {
		switch (header) {

		case 0x00:
			log(errorLog, "Unexpected header " + header + ": 0x" + Integer.toHexString(data));
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

		case 0x03:
			if (NEW_TEST) {
				distanceLeftShortList.add(new TimeValuePair(System.currentTimeMillis(), data));
			} else {
				//leftChart.update(data, !paused);
			}
			break;

		case 0x04:
			if (NEW_TEST) {
				distanceLeftLongList.add(new TimeValuePair(System.currentTimeMillis(), data));
			} else {
				//rightChart.update(data, !paused);
			}
			break;

		case 0x05:
			if (NEW_TEST) {
				distanceForwardLeftList.add(new TimeValuePair(System.currentTimeMillis(), data));
			} else {
				//forwardRightChart.update(data, !paused);
			}
			break;

		case 0x06:
			if (NEW_TEST) {
				distanceForwardCenterList.add(new TimeValuePair(System.currentTimeMillis(), data));
			} else {
				//forwardLeftChart.update(data, !paused);
			}
			break;

		case 0x07:
			if (NEW_TEST) {
				distanceForwardRightList.add(new TimeValuePair(System.currentTimeMillis(), data));
			} else {
				//forwardChart.update(data, !paused);				
			}
			break;

		case 0x08:
			if (NEW_TEST) {
				distanceRightLongList.add(new TimeValuePair(System.currentTimeMillis(), data));
			}
			break;

		case 0x09:
			if (NEW_TEST) {
				distanceRightShortList.add(new TimeValuePair(System.currentTimeMillis(), data));
			}
			break;

		case 0x0A:
			// TODO: reglerfel
			log(errorLog, "Error code 0x" + Integer.toHexString(data));
			break;

		case 0x0B:
			if (NEW_TEST) {
				tapeList.add(new TimeValuePair(System.currentTimeMillis(), data));
			}
			break;
			
		case 0x0C:
			log(errorLog, "Error code 0x" + Integer.toHexString(data));
			break;
			
		case 0x0D:
			// TODO: ping
			if (data != 0x00) {
				log(errorLog, "Invalid ping data: 0x" + Integer.toHexString(data));
			}
			break;

		default:
			log(errorLog, "Invalid header: 0x" + Integer.toHexString(header) + ", data: 0x" + Integer.toHexString(data));
			break;
		}
	}

	/*
	 * From: http://en.wikipedia.org/wiki/Hamming_distance
	 */
	int hammingDistance(int x, int y)
	{
		int dist = 0;
		int val = x ^ y;

		// Count the number of set bits
		while(val != 0)
		{
			++dist; 
			val &= val - 1;
		}

		return dist;
	}
}
