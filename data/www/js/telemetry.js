var chart;
var chart_length = 200;
var max_rate = 5 // hz
var last_update = Date.now();

function create_chart(keys) {
   var series = [];
   keys.forEach(function(key) {
      let name = key.replace('_',' ');
      series.push({
         showInLegend: true, 
         name: key,
         legendText: name,
         type : 'line',
         dataPoints: []
      });
   });
   chart = new CanvasJS.Chart("telemetry_chart", {
		title:{
			text: undefined              
		},
		data: series
	});
	chart.render();
}

if (!!window.EventSource) {
   var source = new EventSource('/events');

   source.addEventListener('open', function (e) {
      console.log("ESPturismo events connected");
   }, false);

   source.addEventListener('error', function (e) {
      if (e.target.readyState != EventSource.OPEN) {
         console.log("ESPturismo events disconnected");
      }
   }, false);

   source.addEventListener('message', function (e) {
      console.log("message", e.data);
   }, false);

   source.addEventListener('telemetry', function (e) {
      // create chart if it doesnt exist
      var data = JSON.parse(e.data);
      var keys = Object.keys(data)
      if (!chart)
         create_chart(keys);
      
      var time = (new Date()).getTime();
      if ((time - last_update) < (1000 / max_rate)) {

      } else {
         var length = chart.options.data[0].dataPoints.length;
         for (var i = 0; i < keys.length; i++) {
            let value = Number(data[keys[i]]);
            if(length > chart_length)
               chart.options.data[i].dataPoints.shift()
            chart.options.data[i].dataPoints.push({x:time, y:value});
         }
         chart.render();
         last_update = time;
      }

   });
}