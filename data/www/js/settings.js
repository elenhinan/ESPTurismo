$(document).ready(function () {
   $.ajaxSetup({ cache: false });

   // set values from json data
   get_settings();

   // send values
   $("#apply").click(set_settings);

   // tell espturismo to save settings to flash
   $("#save").click(function () {
      $.getJSON("/rest/save_settings", function (data) {
         alert(data['save'] == true ? "Saved succesfully" : "Save failed!");
      })
   });
   // tell espturismo to reboot
   $("#reboot").click(function () {
      $.getJSON("/rest/reboot", function (data) {
         alert(data['reboot'] == true ? "Rebooting, wait a bit" : "Reboot failed!");
      })
   });
   // enable tooltips
   $('[data-toggle="tooltip"]').tooltip()
});

function get_settings() {
   $.getJSON("/rest/settings", function (data) {
      Object.keys(data).forEach(function (group) {
         Object.keys(data[group]).forEach(function (key) {
            $(`#${group}\\.${key}`).val(data[group][key])
         });
      });
   });
}

function set_settings() {
   settings = {}
   $(":input").each(function (index) {
      // get id, value, group and key for each input
      let id = $(this)[0].id;
      let value = $(this)[0].value;

      let group, key;
      [group, key] = id.split('.');

      // add to settings neste dict if value is present
      if (value) {
         let num_value = Number(value);
         if (!settings[group])
            settings[group] = {};
         // send numbers as numbers, not string
         if (isNaN(num_value))
            settings[group][key] = value;
         else
            settings[group][key] = num_value;
      }
   });
   $.ajax({
      url: '/rest/settings',
      type: 'POST',
      dataType: "json",
      data: JSON.stringify(settings),
      contentType: 'application/json',
      success: function (msg) { alert(msg); }
   });
}