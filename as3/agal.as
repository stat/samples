package gs.agal
{

import flash.display.DisplayObjectContainer;
import flash.display.Stage;
import flash.display3D.Context3D;
import flash.events.Event;
import flash.utils.Dictionary;

public final class AGAL
{

    private static var _instance:AGAL;
    public static function get Instance():AGAL
    {
        if(!_instance)
        {
            _instance = new AGAL();
        }

        return _instance;
    }

    public static function Initialize(displayContainer:DisplayObjectContainer):void
    {
        Instance.initialize(displayContainer);
    }

    public static function AttachClient(client:IAGALClient):void
    {
        return Instance.attachClient(client);
    }

    public static function DetachClient(client:IAGALClient):void
    {
        return Instance.detachClient(client);
    }

    //--------------------------------------------------------------------------
    //
    //  Variables
    //
    //--------------------------------------------------------------------------

    private var clients:Dictionary;
    private var contexts:Vector.<Context3D>;

    private var displayContainer:DisplayObjectContainer;
    private var stage:Stage;

    private var width:uint;
    private var height:uint;

    //--------------------------------------------------------------------------
    //
    //  Constructor
    //
    //--------------------------------------------------------------------------

    /**
     *  Constructor.
     *
     *  @langversion 3.0
     *  @playerversion Flash 9
     *  @playerversion AIR 1.1
     *  @productversion GS Platform 1.0
     */
    public function AGAL()
    {
        clients = new Dictionary(true);
        contexts = new Vector.<Context3D>();
    }

    public function initialize
    (
        obj:DisplayObjectContainer,
        width:uint = 1280,
        height:uint = 720
    )
    :void
    {
        if(displayContainer == obj)
        {
            return;
        }
        else
        {
            invalidateClients();
        }

        displayContainer = obj;

        this.width = width;
        this.height = height;

        if(!displayContainer.stage)
        {
            displayContainer.addEventListener(Event.ADDED_TO_STAGE, initStage3D);
        }
        else
        {
            initStage3D();
        }
    }

    public function attachClient(client:IAGALClient):void
    {
        var n:uint = contexts.length;
        var context:Context3D = n ? contexts[n-1] : null ;

        clients[client] = {} ;

        trace("attaching client", client, contexts.length);

        if (contexts.length)
        {
            client.context = context;
            client.validate();
        }
    }

    public function detachClient(client:IAGALClient):void
    {
        // client removal nullifies its AGAL properties such as:
        //  textures
        //  programs
        //  ...

        if(!clients[client])
        {
            return;
        }

        client.invalidate();
        client.context = null;
    }

    public function invalidateClients():void
    {
        var k:*;
        for(k in clients)
        {
            k.context = null;
            k.invalidate();
        }
    }

    public function validateClients():void
    {
        if(!contexts.length)
        {
            throw new Error("AGAL cannot validate its clients without a context!");
        }

		trace("validating clients");

        var k:*;
        for(k in clients)
        {
            trace("client:", k);
            k.context = contexts[0];
            k.validate();
        }
    }

    private function initGL(event:Event = null):void
    {
        var index:uint = contexts.length;
        var context:Context3D = stage.stage3Ds[index].context3D;

        if(!context)
        {
            throw new Error("3D Context could not be created!");
        }

        context.configureBackBuffer(width,  height, 0, false);
        contexts.push(context);

        validateClients();
    }

    private function initStage3D(event:Event = null):void
    {
        var index:uint = contexts.length;

        stage = displayContainer.stage;
        stage.stage3Ds[index].
            addEventListener(Event.CONTEXT3D_CREATE, initGL);

        stage.stage3Ds[index].requestContext3D();
    }

}

}
