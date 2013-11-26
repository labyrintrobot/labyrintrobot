package application;

import javax.bluetooth.ServiceRecord;

import application.BluetoothDiscoveryListener.IServicesDiscoveredCallback;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;

public class BluetoothStage extends Application {
	
	private ServiceRecord service;
	private final BluetoothDiscoveryListener bluetoothDiscoveryListener;
	private final Button startButton;
	private final ProgressIndicator progressIndicator;

	public BluetoothStage() {
		bluetoothDiscoveryListener = new BluetoothDiscoveryListener("FireFly-A696", new IServicesDiscoveredCallback() {

			@Override
			public void servicesDiscovered(ServiceRecord service) {
				BluetoothStage.this.service = service;
				startButton.setDisable(false);
				Platform.runLater(new Runnable() {
					
					@Override
					public void run() {
						progressIndicator.setProgress(2.0);
					}
				});
			}
		});
		bluetoothDiscoveryListener.findDevices();
		startButton = new Button("Start");
		progressIndicator = new ProgressIndicator();
	}
	
	@Override
	public void start(final Stage primaryStage) {

		try {
			primaryStage.setTitle("Labyrintrobot");

			BorderPane root = new BorderPane();
			HBox southBox = new HBox();
			southBox.getChildren().addAll(startButton, progressIndicator);
			root.setBottom(southBox);
			southBox.setAlignment(Pos.BOTTOM_CENTER);
			
			progressIndicator.setProgress(-1);
			
			Scene scene = new Scene(root, 320, 320);
			primaryStage.setScene(scene);
			primaryStage.show();
			
			startButton.setDisable(true);
			
			startButton.setOnAction(new EventHandler<ActionEvent>() {
				@Override public void handle(ActionEvent e) {
					Stage stage = new PresentationStage(BluetoothStage.this.service);
		            stage.show();
		            primaryStage.hide();
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
