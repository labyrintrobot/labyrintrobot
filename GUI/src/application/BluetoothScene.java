package application;

import application.BluetoothDiscoveryListener.IBluetoothDeviceDiscovered;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.scene.layout.BorderPane;

public class BluetoothScene extends Application {
	
	private final BluetoothDiscoveryListener bluetoothDiscoveryListener;
	private final ListView<String> deviceList;
	private final Button startButton;

	public BluetoothScene() {
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
		deviceList = new ListView<String>();
		startButton = new Button("Start");
	}
	
	@Override
	public void start(Stage primaryStage) {

		try {
			primaryStage.setTitle("Labyrintrobot");

			BorderPane root = new BorderPane();
			root.setCenter(deviceList);
			root.setBottom(startButton);
			
			Scene scene = new Scene(root, 320, 320);
			primaryStage.setScene(scene);
			primaryStage.show();
			
			Runnable btRunnable = new Runnable() {
				
				@Override
				public void run() {
					bluetoothDiscoveryListener.start();
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
					String str = deviceList.getSelectionModel().getSelectedItem();
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
