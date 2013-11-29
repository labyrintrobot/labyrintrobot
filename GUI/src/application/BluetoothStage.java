package application;

import java.io.IOException;

import javax.bluetooth.BluetoothStateException;

import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;

public class BluetoothStage extends Application {
	
	//private String bluetoothUrl;
	//private final BluetoothDiscoveryListener bluetoothDiscoveryListener;
	private final Button startButton;
	private final ProgressIndicator progressIndicator;

	public BluetoothStage() {
		/*bluetoothDiscoveryListener = new BluetoothDiscoveryListener("FireFly-A696", new IServicesDiscoveredCallback() {

			@Override
			public void servicesDiscovered(String bluetoothUrl) {
				BluetoothStage.this.bluetoothUrl = bluetoothUrl;
				startButton.setDisable(false);
				Platform.runLater(new Runnable() {
					
					@Override
					public void run() {
						progressIndicator.setProgress(2.0);
					}
				});
			}
		});
		bluetoothDiscoveryListener.findDevices();*/
		startButton = new Button("Start");
		progressIndicator = new ProgressIndicator();
	}
	
	@Override
	public void start(final Stage primaryStage) {

		try {
			primaryStage.setTitle("Labyrintrobot");

			BorderPane root = new BorderPane();
			HBox southBox = new HBox();
			southBox.getChildren().addAll(startButton/*, progressIndicator*/);
			root.setBottom(southBox);
			southBox.setAlignment(Pos.BOTTOM_CENTER);
			
			Scene scene = new Scene(root, 320, 120);
			primaryStage.setScene(scene);
			primaryStage.show();
			
			//startButton.setDisable(true);
			
			startButton.setOnAction(new EventHandler<ActionEvent>() {
				@Override public void handle(ActionEvent e) {
					//Stage stage = new PresentationStage(bluetoothUrl);
					Stage stage;
					try {
						stage = new PresentationStage("btspp://00066603A696:1;authenticate=true;encrypt=false;master=false");
						stage.show();
						primaryStage.hide();
					} catch (IOException e1) {
						System.err.println("Unable to open Bluetooth connection");
						//e1.printStackTrace();
					}
				}
			});

		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	public static void main(String[] args) {
		launch(args);
	}
}
