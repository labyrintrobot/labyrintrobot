package application;

import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class Main extends Application implements BluetoothAdapter.IMessageReceiver {

	private static final String PAUSE_TEXT = "Pause";
	private static final String RESUME_TEXT = "Resume";

	private ControllerAdapter controllerAdapter;
	private BluetoothAdapter bluetoothAdapter;

	private RealTimeChart leftChart;
	private RealTimeChart rightChart;
	private RealTimeChart forwardLeftChart;
	private RealTimeChart forwardRightChart;
	private RealTimeChart forwardChart;
	private RealTimeChart gyroChart;
	
	private TextArea signalLog;
	private TextArea errorLog;

	private boolean paused = false;

	@Override
	public void start(Stage primaryStage) {

		bluetoothAdapter = new BluetoothAdapter(this);
		controllerAdapter = new ControllerAdapter(bluetoothAdapter);

		try {
			primaryStage.setTitle("Labyrintrobot");
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

			rightChart = new RealTimeChart("Right");
			forwardRightChart = new RealTimeChart("Forward, right");
			forwardChart = new RealTimeChart("Forward");
			forwardLeftChart = new RealTimeChart("Forward, left");
			leftChart = new RealTimeChart("Left");
			gyroChart = new RealTimeChart("Gyro");

			errorLog = new TextArea();
			errorLog.setDisable(true);
			signalLog = new TextArea();
			signalLog.setDisable(true);

			eastBox.getChildren().add(rightChart.getUnderLyingLineChart());
			westBox.getChildren().add(leftChart.getUnderLyingLineChart());
			northBox.getChildren().addAll(forwardLeftChart.getUnderLyingLineChart(), forwardChart.getUnderLyingLineChart(), forwardRightChart.getUnderLyingLineChart());
			southBox.getChildren().add(signalLog);
			centerBox.getChildren().addAll(pauseButton, signalLog);

			//leftChart.addToLineChart(7);
			//leftChart.addToLineChart(8);
			//leftChart.addToLineChart(9);

			BorderPane root = new BorderPane();
			root.setRight(eastBox);
			root.setTop(northBox);
			root.setLeft(westBox);
			root.setBottom(signalLog);
			root.setCenter(pauseButton);
			Scene scene = new Scene(root, 1280, 1024);
			primaryStage.setScene(scene);

			EventHandler<KeyEvent> pressEvent = new EventHandler<KeyEvent>() {

				@Override
				public void handle(KeyEvent t) {
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
				}
			};

			EventHandler<KeyEvent> releaseEvent = new EventHandler<KeyEvent>() {

				@Override
				public void handle(KeyEvent t) {
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

			scene.setOnKeyPressed(pressEvent);
			scene.setOnKeyReleased(releaseEvent);

			pauseButton.setOnKeyPressed(pressEvent);
			pauseButton.setOnKeyReleased(releaseEvent);

			primaryStage.show();

			listen();

		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	private void listen() {
		Thread listenerThread = new Thread(new Runnable() {

			@Override
			public void run() {
				bluetoothAdapter.receiveMessages();
			}

		});

		listenerThread.start();
	}

	public static void main(String[] args) {
		launch(args);
	}

	private void log(TextArea ta, String text) {
		ta.setText(signalLog.getText() + text + '\n');
	}

	@Override
	public void receiveMessage(byte header, byte data) {
		// TODO concurrent
		switch (header) {
		case 0x01:
			String text;
			switch (data) {
			case 0x00:
				text = "Driving forward";
				break;
			case 0x01:
				text = "Driving backward";
				break;
			case 0x02:
				text = "Turning right";
				break;
			case 0x03:
				text = "Turning left";
				break;
			case 0x04:
				text = "Rotating right";
				break;
			case 0x05:
				text = "Rotating left";
				break;
			case 0x06:
				text = "Stop";
				break;
			default:
				text = "Illegal data received for header " + header + ": " + Integer.toHexString(data);
				break;
			}
			log(signalLog, text);
			break;
		case 0x03:
			if (paused == false) {
				leftChart.update(data);
			}
			break;
		case 0x04:
			if (paused == false) {
				rightChart.update(data);
			}
			break;
		case 0x05:
			if (paused == false) {
				forwardRightChart.update(data);
			}
			break;
		case 0x06:
			if (paused == false) {
				forwardLeftChart.update(data);
			}
			break;
		case 0x07:
			if (paused == false) {
				forwardChart.update(data);
			}
			break;
		case 0x08:
			// TODO
			break;
		case 0x09:
			// TODO
			break;
		case 0x0A:
			log(errorLog, "Error code " + Integer.toHexString(data));
			break;
		case 0x0B:
			if (data == 0x00) {
				bluetoothAdapter.sendMessage(header, data);
			} else {
				// TODO
			}
			break;
		default:
			break;
		}
	}
}
