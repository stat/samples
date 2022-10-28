using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Threading;
using System.Text;

using Id.Common.Net.TCP;

using Id.Tracker;
using Id.Tracker.Core;


namespace Id.DataProviders.FLOSC
{

public class FloscSender : IOuputProvider
{

    #region Events
    override public event TrackerHookErrorHandler TrackerHookError;
    #endregion


    #region Fields

    FloscSettings _settings;

    List<long> _blobs;

    
    TcpServer _tcpServer;
    IPEndPoint _ipEndPoint;

    #endregion


    #region Constructors, destructors and initializers

    public FloscSender()
        : this(IPAddress.Loopback, 3000, 1680, 1050)
    {
    }

    public FloscSender(object settings)
    {
        _settings = settings as FloscSettings;

        if (_settings == null)
        {
            throw new ArgumentNullException("floscSettings", "settings are incompatable with this object");
        }

        Console.WriteLine(_settings.Width);
        _ipEndPoint = new IPEndPoint(IPAddress.Parse(_settings.IPAddress), int.Parse(_settings.Port));
        initialize();
    }

    public FloscSender(IPAddress ipAddress, int port, int width, int height)
    {
        _settings = new FloscSettings();
        _settings.IPAddress = ipAddress.ToString();
        _settings.Port = port.ToString();
        //_settings.Width = ;
        _settings.Height = height;

        _ipEndPoint = new IPEndPoint(ipAddress, port);
        initialize();
    }

    private void initialize()
    {
        _blobs = new List<long>();

        _tcpServer = new TcpServer(_ipEndPoint.Address, _ipEndPoint.Port);
        _tcpServer.Start();

        this.TrackerChanged += new TrackerChangeHandler(onTrackerChanged);
    }

    public override void Dispose()
    {
        _tcpServer.Dispose();
    }

    #endregion


    #region Private Methods

    override protected void tracker_deQueueCompleteHandler(List<Blob> additionQueue, List<Blob> removalQueue, List<Blob> updateQueue)
    {
        if (updateQueue == null && additionQueue == null && removalQueue == null)
            return;

        if (additionQueue != null && updateQueue == null)
            updateQueue = new List<Blob>();

        // Update the local collection
        if(additionQueue != null)
        additionQueue.ForEach(delegate(Blob blob)
        {
            _blobs.Add(blob.id);

            try
            {
                updateQueue.Add(blob);
            }
            catch (Exception e)
            {
                Debug.WriteLine("error: " + e.Message);
            }
        });

        if(removalQueue != null)
        removalQueue.ForEach(delegate(Blob blob)
        {
            _blobs.Remove(blob.id);
        });

        _blobs.Sort();

        // Build the strings
        StringBuilder aliveList = new StringBuilder();
        StringBuilder setList = new StringBuilder();

        _blobs.ForEach(delegate(long id)
        {
            aliveList.Append("<ARGUMENT TYPE='i' VALUE='" + id + "' />");
        });

        if(updateQueue != null)
        updateQueue.ForEach(delegate(Blob blob)
        {
            setList.Append(
                "<MESSAGE NAME='/tuio/2Dcur'>" +
                    "<ARGUMENT TYPE='s' VALUE='set' />" +
                    "<ARGUMENT TYPE='i' VALUE='" + blob.id + "' />" +
                    "<ARGUMENT TYPE='f' VALUE='" + (blob.x / _settings.Width) + "' />" +
                    "<ARGUMENT TYPE='f' VALUE='" + (blob.y / _settings.Height) + "' />" +
                    "<ARGUMENT TYPE='f' VALUE='0' />" +
                    "<ARGUMENT TYPE='f' VALUE='0' />" +
                    "<ARGUMENT TYPE='f' VALUE='0' />" +
                "</MESSAGE>"
            );

        });

        // Build The XML String
        StringBuilder xmlOSC = new StringBuilder(
        "<OSCPACKET ADDRESS='127.0.0.1' PORT='1173' TIME='-3614119407970922408'>" +
            "<MESSAGE NAME='/tuio/2Dcur'>" +
                "<ARGUMENT TYPE='s' VALUE='source' />" +
                "<ARGUMENT TYPE='s' VALUE='vision' />" +
            "</MESSAGE>" +

            "<MESSAGE NAME='/tuio/2Dcur'>" +
                "<ARGUMENT TYPE='s' VALUE='alive' />" + aliveList +
            "</MESSAGE>"

            + setList +

            "<MESSAGE NAME='/tuio/2Dcur'>" +
                "<ARGUMENT TYPE='s' VALUE='fseq' />" +
                "<ARGUMENT TYPE='i' VALUE='-1' />" +
            "</MESSAGE>" +
        "</OSCPACKET>\0"
        );

        _tcpServer.Send(xmlOSC.ToString());
    }

    #endregion


    #region Inherited Methods::ITrackerHook

    public override void Start()
    {
        throw new NotImplementedException();
    }

    public override void Stop()
    {
        throw new NotImplementedException();
    }

    public override void Restart()
    {
        throw new NotImplementedException();
    }

    override protected void onTrackerHookError(object sender, string message, System.Diagnostics.StackTrace stack)
    {
        if (TrackerHookError != null)
        {
            TrackerHookError(sender, message, stack);
        }
    }

    public override string GetModuleInfo()
    {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.Append(
            "FLOSC: \r\n" + 
            "   IP Address: " + _ipEndPoint.Address.ToString() + "\r\n" +
            "   Port: " + _ipEndPoint.Port.ToString()
        );

        return stringBuilder.ToString();
    }

    public override void ShowModuleInfo()
    {
        throw new NotImplementedException();
    }

    #endregion

}

}
