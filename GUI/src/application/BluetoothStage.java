package application;

import application.BluetoothDiscoveryListener.IBluetoothDeviceDiscovered;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;

public class BluetoothStage extends Application {
	
	private final BluetoothDiscoveryListener bluetoothDiscoveryListener;
	private final ListView<String> deviceList;
	private final Button startButton;
	private final ProgressIndicator progressIndicator;

	public BluetoothStage() {
		bluetoothDiscoveryListener = new BluetoothDiscoveryListener(new IBluetoothDeviceDiscovered() {
			
			@Override
			public void deviceDiscovered(final String name) {
				Platform.runLater(new Runnable() {
					
					@Override
					public void run() {
						deviceList.getItems().add(name);
					}
				});
			}
		});
		bluetoothDiscoveryListener.findDevices();
		deviceList = new ListView<String>();
		startButton = new Button("Start");
		progressIndicator = new ProgressIndicator();
	}
	
	@Override
	public void start(final Stage primaryStage) {

		try {
			primaryStage.setTitle("Labyrintrobot");

			BorderPane root = new BorderPane();
			root.setCenter(deviceList);
			HBox southBox = new HBox();
			southBox.getChildren().addAll(startButton, progressIndicator);
			root.setBottom(southBox);
			southBox.setAlignment(Pos.BOTTOM_CENTER);
			
			progressIndicator.setProgress(-1);
			
			Scene scene = new Scene(root, 320, 320);
			primaryStage.setScene(scene);
			primaryStage.show();
			
			Runnable btRunnable = new Runnable() {
				
				@Override
				public void run() {
					//bluetoothDiscoveryListener.start();
					Platform.runLater(new Runnable() {
						
						@Override
						public void run() {
							progressIndicator.setProgress(2.0); // Finished
						}
					});
				}
			};
			Thread btThread = new Thread(btRunnable);
			btThread.start();
			
			startButton.setDisable(true);
			
			deviceList.getSelectionModel().selectedItemProperty().addListener(new ChangeListener<String>() {

			    @Override
			    public void changed(ObservableValue<? extends String> observable, String oldValue, String newValue) {
			    	startButton.setDisable(newValue == null);
			    }
			});
			
			startButton.setOnAction(new EventHandler<ActionEvent>() {
				@Override public void handle(ActionEvent e) {
					String bluetoothServerUrl = deviceList.getSelectionModel().getSelectedItem();
					
					Stage stage = new PresentationStage(bluetoothServerUrl);
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
