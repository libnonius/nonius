!function () {
    var data = [
        {% for benchmark in benchmarks %}
        {
            name: '{$benchmark.name}',
            type: 'scatter',
            mode: 'markers',
            marker: { symbol: {$loop.index0} },
            x: [{% for time in benchmark.times %} {$loop.index}, {% endfor %}],
            y: [{% for time in benchmark.times %} {$time}, {% endfor %}],
        },
        {% endfor %}
    ];

    var layout = {
        title: '{$title}',
        showleyend: true,
        xaxis: { title: 'Measurement' },
        yaxis: { title: 'Time ({$units})' },
        legend: {
            font: { family: 'monospace' },
            borderwidth: 2,
            bordercolor: '#777'
        }
    };

    var plotdiv = document.getElementById("all-together");
    Plotly.newPlot(plotdiv, data, layout);
    window.addEventListener("resize", function() {
        Plotly.Plots.resize(plotdiv);
    });
}();
