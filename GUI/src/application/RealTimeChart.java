package application;

import java.util.LinkedList;
import java.util.Set;

import javafx.collections.ObservableList;
import javafx.scene.Node;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;

public class RealTimeChart {
	private final LineChart<Number, Number> underlyingLineChart;
	
	private static final int DATA_MAX = 200;
	private static final int QUEUE_THRESHOLD = 20;
	
	private int xSeriesData = 0;
	
	private final LinkedList<Integer> queue = new LinkedList<>();
	
	public RealTimeChart(String name, String yAxisText, int maxY) {
		NumberAxis xAxis = new NumberAxis(0, DATA_MAX, DATA_MAX / 8);
        NumberAxis yAxis = new NumberAxis(0, maxY, maxY/8);
        xAxis.setLabel("Time");
        yAxis.setLabel(yAxisText);
        xAxis.setForceZeroInRange(false);
        underlyingLineChart = new LineChart<Number,Number>(xAxis, yAxis);
        underlyingLineChart.setAnimated(false);
        underlyingLineChart.setCreateSymbols(false);
        
        underlyingLineChart.setMaxHeight(360);
        
        underlyingLineChart.setTitle(name);
        underlyingLineChart.setLegendVisible(false);
        XYChart.Series<Number, Number> series = new XYChart.Series<Number, Number>();
        series.setName(name);
        
        underlyingLineChart.getData().add(series);
        
        Set<Node> lookupAll = underlyingLineChart.lookupAll(".series0");
        for (Node n : lookupAll) {
        	n.setStyle("-fx-stroke-width: 1px;-fx-stroke: black;");
        }
	}
	
	public void update(int x, boolean flush) {
        
		queue.addLast(x);
		
		if (flush && queue.size() > QUEUE_THRESHOLD) {
        	flush();
        } else if (queue.size() > QUEUE_THRESHOLD) {
        	queue.removeFirst();
        }
	}
	
	public void flush() {
        XYChart.Series<Number, Number> series = underlyingLineChart.getData().get(0);
        ObservableList<XYChart.Data<Number, Number>> data = series.getData();
        
        for (Integer i : queue) {
        	data.add(new XYChart.Data<Number, Number>(xSeriesData++, i));
        }
        queue.clear();
        
        while (series.getData().size() > DATA_MAX) {
            series.getData().remove(0, series.getData().size() - DATA_MAX);
        }
        
        NumberAxis xAxis = (NumberAxis) underlyingLineChart.getXAxis();
        xAxis.setLowerBound(xSeriesData-DATA_MAX);
        xAxis.setUpperBound(xSeriesData-1);
	}
	
	public LineChart<Number, Number> getUnderLyingLineChart() {
		return underlyingLineChart;
	}
}
