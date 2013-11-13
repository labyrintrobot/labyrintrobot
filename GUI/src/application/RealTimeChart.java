package application;

import javafx.collections.ObservableList;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;

public class RealTimeChart {
	private final LineChart<Number, Number> underlyingLineChart;
	
	private static final int DATA_MAX = 10;
	private int xSeriesData = 0;
	
	public RealTimeChart(String name) {
		NumberAxis xAxis = new NumberAxis(0, DATA_MAX, DATA_MAX / 8);
        NumberAxis yAxis = new NumberAxis();
        xAxis.setLabel("Time");
        xAxis.setForceZeroInRange(false);
        underlyingLineChart = new LineChart<Number,Number>(xAxis, yAxis);
        
        underlyingLineChart.setTitle(name);
        underlyingLineChart.setLegendVisible(false);
        XYChart.Series<Number, Number> series = new XYChart.Series<Number, Number>();
        series.setName(name);
        
        underlyingLineChart.getData().add(series);
	}
	
	public void addToLineChart(int x) {
        //series.setName(name);

        XYChart.Series<Number, Number> series = underlyingLineChart.getData().get(0);
        ObservableList<XYChart.Data<Number, Number>> data = series.getData();
        data.add(new XYChart.Data<Number, Number>(xSeriesData++, x));
        if (series.getData().size() > DATA_MAX) {
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
