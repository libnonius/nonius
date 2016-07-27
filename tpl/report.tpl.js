$(function () {
    $('#all-together').highcharts({
        chart: {
            type: 'scatter',
            zoomType: 'y'
        },
        title: {
            text: '{$title}'
        },
        subtitle: {
            text: 'generated with <a href="http://flamingdangerzone.com/nonius">nonius</a>'
        },
        yAxis: {
            title: { text: 'Time ({$units})' },
            startOnTick: false,
            endOnTick: true,
            showLastLabel: true,
        },
        xAxis: {
            title: { text: null },
            startOnTick: true,
            endOnTick: true,
            showLastLabel: true,
            minPadding: 0,
            maxPadding: 0
        },
        legend: {
            layout: 'vertical',
            align: 'right',
            verticalAlign: 'top',
            x: 0,
            y: 100,
            backgroundColor: '#FFFFFF',
            borderWidth: 1
        },
        plotOptions: {
            scatter: {
                marker: {
                    radius: 5,
                    states: {
                        hover: {
                            enabled: true,
                            lineColor: 'rgb(100,100,100)'
                        }
                    }
                },
                states: {
                    hover: {
                        marker: {
                            enabled: false
                        }
                    }
                },
                tooltip: {
                    headerFormat: '<b>{series.name}</b><br>',
                    pointFormat: 'Run #{point.x}: <b>{point.y} {$units}</b>'
                }
            }
        },
        series: [{% for benchmark in benchmarks %}{
                    name: '{$benchmark.name}',
                    data: [{% for time in benchmark.times %}[{$time}],{% endfor %}]
                },
                {% endfor %}
        ]
    });
});
