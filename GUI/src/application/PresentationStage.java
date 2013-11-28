package application;

import java.io.IOException;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

public class PresentationStage extends Stage implements BluetoothAdapter.IMessageReceiver {

	private static final String PAUSE_TEXT = "Pause";
	private static final String RESUME_TEXT = "Resume";

	private final ControllerAdapter controllerAdapter;
	private final BluetoothAdapter bluetoothAdapter;
	private final ControlPad controlPad;

	private final RealTimeChart leftChart;
	private final RealTimeChart rightChart;
	private final RealTimeChart forwardLeftChart;
	private final RealTimeChart forwardRightChart;
	private final RealTimeChart forwardChart;
	private final RealTimeChart gyroChart;
	private final RealTimeChart tapeChart;

	private final TextArea errorLog;

	private boolean paused = false;

	public PresentationStage(String bluetoothUrl) {

		bluetoothAdapter = new BluetoothAdapter(this);


		controllerAdapter = new ControllerAdapter(bluetoothAdapter);
		controlPad = new ControlPad();

		rightChart = new RealTimeChart("Right", "Distance", 256);
		forwardRightChart = new RealTimeChart("Forward, right", "Distance", 256);
		forwardChart = new RealTimeChart("Forward", "Distance", 256);
		forwardLeftChart = new RealTimeChart("Forward, left", "Distance", 256);
		leftChart = new RealTimeChart("Left", "Distance", 256);
		gyroChart = new RealTimeChart("Gyro", "Angular rate", 256);
		tapeChart = new RealTimeChart("Tape sensor", "Hamming distance", 8);

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


			eastBox.getChildren().add(rightChart.getUnderLyingLineChart());
			westBox.getChildren().add(leftChart.getUnderLyingLineChart());
			northBox.getChildren().addAll(forwardLeftChart.getUnderLyingLineChart(), forwardChart.getUnderLyingLineChart(), forwardRightChart.getUnderLyingLineChart());
			southBox.getChildren().addAll(errorLog, tapeChart.getUnderLyingLineChart(), gyroChart.getUnderLyingLineChart());
			centerBox.getChildren().addAll(pauseButton, controlPad);

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
					rightChart.flush();
					forwardRightChart.flush();
					forwardChart.flush();
					forwardLeftChart.flush();
					leftChart.flush();
					gyroChart.flush();
					tapeChart.flush();
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

		listenerThread.start();
	}

	private void log(TextArea ta, String text) {
		ta.setText(ta.getText() + text + '\n');
		ta.positionCaret(ta.getText().length()); // Scroll to end
	}

	@Override
	public void receiveMessage(int header, int data) {
		switch (header) {

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
				log(errorLog, "Illegal data received for header " + header + ": 0x" + Integer.toHexString(data));
				break;
			}
			break;

		case 0x03:
			leftChart.update(data, !paused);
			break;

		case 0x04:
			rightChart.update(data, !paused);
			break;

		case 0x05:
			forwardRightChart.update(data, !paused);
			break;

		case 0x06:
			forwardLeftChart.update(data, !paused);
			break;

		case 0x07:
			forwardChart.update(data, !paused);
			break;

		case 0x08:
			if ((data & 0x80) == 0) {
				int hamDist = hammingDistance(0, data);
				tapeChart.update(hamDist, !paused);
			} else {				
				log(errorLog, "Invalid tape sensor data: 0x" + Integer.toHexString(data));
			}
			break;

		case 0x09:
			gyroChart.update(data, !paused);
			break;

		case 0x0A:
			log(errorLog, "Error code 0x" + Integer.toHexString(data));
			break;

		case 0x0B:
			if (data == 0x00) {
				try {
					bluetoothAdapter.sendMessage(header, data);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else {
				log(errorLog, "Invalid ping data: 0x" + Integer.toHexString(data));
			}
			break;

		default:
			log(errorLog, "Invalid header: 0x" + Integer.toHexString(data));
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
