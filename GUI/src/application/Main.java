package application;
	
import javafx.application.Application;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class Main extends Application {
	
	@Override
	public void start(Stage primaryStage) {
		try {
			primaryStage.setTitle("Labyrintrobot");
	        Button btn = new Button();
	        btn.setText("Pause");
	       
	        VBox eastBox = new VBox();
	        VBox westBox = new VBox();
	        HBox northBox = new HBox();
	        
	        RealTimeChart leftChart = new RealTimeChart("left");
	        RealTimeChart rightChart = new RealTimeChart("right");
	        RealTimeChart forwardLeftChart = new RealTimeChart("forward, left");
	        RealTimeChart forwardRightChart = new RealTimeChart("forward, right");
	        RealTimeChart forwardChart = new RealTimeChart("forward");
	        
	        eastBox.getChildren().add(rightChart.getUnderLyingLineChart());
	        westBox.getChildren().add(leftChart.getUnderLyingLineChart());
	        northBox.getChildren().addAll(forwardLeftChart.getUnderLyingLineChart(), forwardChart.getUnderLyingLineChart(), forwardRightChart.getUnderLyingLineChart());
	        
	        leftChart.addToLineChart(7);
	        leftChart.addToLineChart(8);
	        leftChart.addToLineChart(9);
	        
	        BorderPane root = new BorderPane();
	        root.setLeft(westBox);
	        root.setRight(eastBox);
	        root.setTop(northBox);
	        root.setCenter(btn);
	        primaryStage.setScene(new Scene(root, 1280, 1024));
	        primaryStage.show();
	        
	        
		} catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void main(String[] args) {
		launch(args);
	}
}
