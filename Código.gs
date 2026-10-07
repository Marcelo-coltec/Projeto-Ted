// Código GAS
function doGet(e) {
  Logger.log( JSON.stringify(e) );
  var result = 'Ok';
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1jXPTgh3pWtJe6MY0_COSoicbSOJY8kbjwVajcz0vxQk'; // Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();
    var newRow = sheet.getLastRow() + 1;
    var rowData = [];
    var Curr_Date = new Date();
    rowData[1] = Curr_Date; // Date in column B
    var Curr_Time = Utilities.formatDate(Curr_Date, "America/Sao_Paulo", 'HH:mm:ss');
    rowData[2] = Curr_Time; // Time in column C
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);

      switch (param) {
        case 'id':
        rowData[0] = value;
        break;
        case 'KmVelocity':
        rowData[3] = value; // KmVelocity in column D
        result = 'KmVelocity Written on column D';
        break;
        case 'temperatureC':
        rowData[5] = value; // Temperature in column F
        result = 'Temperature Written on column F';
        break;
        case 'humidity':
        rowData[6] = value; // Humidity in column G
        result += ' ,Humidity Written on column G';
        break;
        default:
        result = "unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }
  return ContentService.createTextOutput(result);
}

function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}
