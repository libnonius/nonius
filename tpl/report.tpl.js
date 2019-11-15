!function () {
    var data = {
        title: '{$title}',
        units: '{$units}',
        logarithmic: {$logarithmic},
        param: '{$runparam}',
        runs: [
            {% for run in runs %}{
                params: {
                    {% for param in run.params %}'{$param.name}': '{$param.value}',
                    {% endfor %}
                },
                benchmarks: [
                    {% for benchmark in run.benchmarks %}{
                        name: '{$benchmark.name}',
                        {%if benchmark.data %}
                        mean: {$benchmark.data.mean},
                        stddev: {$benchmark.data.stddev},
                        samples: [
                            {% for sample in benchmark.data.samples %}{$sample}, {% endfor %}
                        ],
                        {% endif %}
                    },{% endfor %}
                ]
            },{% endfor %}
        ]
    };
    var origOrder = data.runs[0].benchmarks.map(function (_, i) { return i; })
    var sortOrder = computeSortedOrder();
    var plotdiv = document.getElementById("plot");
    window.addEventListener("resize", function() {
        Plotly.Plots.resize(plotdiv);
    });

    var chooser = document.getElementById("chooser");
    chooser.addEventListener("change", choosePlot);
    chooser.addEventListener("blur", chooser.focus.bind(chooser));
    chooser.focus();

    var isSortedBox = document.getElementById("is-sorted");
    isSortedBox.addEventListener("change", choosePlot);

    var legendStyle = {
        font: { family: 'monospace' },
        borderwidth: 2,
        bordercolor: 'black'
    }

    function zeroes(count) {
        var arr = []
        while (count --> 0) arr.push(0)
        return arr
    }

    function computeSortedOrder() {
        // We sort each run.  Then we compute the "points" of each
        // benchmark as the sum of the positions of this benchmkark on
        // each run.  This gives us a rough indication of which
        // benchmark is best -- the lower the points, the better.
        var runsOrder = data.runs.map(function (r) {
            order = r.benchmarks.map(function (_, i) { return i; })
            order.sort(function (a, b) {
                return r.benchmarks[a].mean - r.benchmarks[b].mean
            })
            return order
        })
        var length = data.runs[0].benchmarks.length
        var points = runsOrder.reduce(function (acc, r) {
            r.forEach(function (elem, idx) {
                acc[elem] += idx
            })
            return acc
        }, zeroes(length))
        var order = data.runs[0].benchmarks.map(function (_, i) { return i; })
        order.sort(function (a, b) {
            return points[a] - points[b]
        })
        return order
    }

    function choosePlot() {
        var plot = chooser.options[chooser.selectedIndex].value
        var order = isSortedBox.checked ? sortOrder : origOrder
        if (plot == 'summary') {
            if (data.runs.length > 1) {
                plotSummary(order);
            } else {
                plotSingleSummary(order);
            }
        } else {
            plotSamples(plot, order);
        }
    }

    function plotSamples(plot, order) {
        var run = data.runs[plot];
        var traces = order.map(function (i) {
            var b = run.benchmarks[i]
            return {
                name: b.name,
                type: 'scatter',
                mode: 'markers',
                marker: { symbol: i },
                y: b.samples,
                x: b.samples && b.samples.map(function (_, i) { return i; })
            }
        });
        var layout = {
            title: data.title,
            showLegend: true,
            xaxis: { title: 'Measurement' },
            yaxis: {
                title: 'Time (' + data.units + ')',
                rangemode: 'tozero',
                zeroline: true
            },
            legend: legendStyle
        };
        Plotly.newPlot(plotdiv, traces, layout);
    }

    function plotSummary(order) {
        var traces = order.map(function (i) {
            return {
                name: data.runs[0].benchmarks[i].name,
                type: 'scatter',
                marker: { symbol: i },
                x: data.runs.map(function (r) { return r.params[data.param]; }),
                y: data.runs.map(function (r) { return r.benchmarks[i].mean; }),
                error_y: {
                    type: 'data',
                    array: data.runs.map(function (r) { return r.benchmarks[i].stddev; }),
                    visible: true
                }
            }
        });
        var layout = {
            title: data.title,
            showLegend: true,
            xaxis: {
                title: data.param,
                type: data.logarithmic ? 'log' : '',
            },
            yaxis: {
                title: 'Time (' + data.units + ')',
                rangemode: 'tozero',
                zeroline: true,
                type: data.logarithmic ? 'log' : '',
            },
            legend: legendStyle
        };
        Plotly.newPlot(plotdiv, traces, layout);
    }

    function plotSingleSummary(order) {
        var traces = order.map(function (i) {
            var b = data.runs[0].benchmarks[i]
            return {
                type: 'bar',
                name: b.name,
                x: [ data.title ],
                y: [ b.mean ],
                error_y: {
                    type: 'data',
                    array: [ b.stddev ],
                    visible: true
                }
            }
        });
        var layout = {
            title: data.title,
            showLegend: true,
            xaxis: {
                title: '',
                showticklabels: false,
            },
            yaxis: {
                title: 'Time (' + data.units + ')',
                rangemode: 'tozero',
                zeroline: true
            },
            legend: legendStyle
        };
        Plotly.newPlot(plotdiv, traces, layout);
    }

    choosePlot();
}();
