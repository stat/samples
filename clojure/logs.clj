(ns archai-consumer.logs
  (:require [taoensso.timbre :as timbre]
            [sentry-clj.core :as sentry]
            [truckerpath.clj-datadog.core :as dd]))

(def sentry-dsn (System/getenv "ARCHAI_CONSUMER_SENTRY_DSN"))
(def datadog-spec (let [host (System/getenv "DATADOG_HOST")
                        port (some-> (System/getenv "DATADOG_PORT")
                                     (not-empty)
                                     (Integer.))]
                    (when (and host port)
                      {:host host
                       :port port})))

(def ^:private timbre->sentry-levels
  {:trace :debug
   :debug :debug
   :info :info
   :warn :warning
   :error :error
   :fatal :fatal
   :report :info})

(when (not-empty sentry-dsn)
  (sentry/init! sentry-dsn)

  (timbre/merge-config!
    {:appenders
     {:sentry
      {:enabled? true
       :async? true
       :min-level :debug
       :rate-limit nil
       :output-fn :inherit
       :fn (fn [{:keys [instant level output_ ?err msg_ ?ns-str context vargs] :as data}]
             (sentry/send-event {:level (get timbre->sentry-levels level)
                                 :logger ?ns-str
                                 :extra context
                                 :message (when-not ?err
                                            (force msg_))
                                 :throwable ?err}))}}}))

#_(when (and (:host datadog-spec)
             (:port datadog-spec))
    (timbre/merge-config!
      {:appenders
       {:datadog
        {:enabled? true
         :async? true
         :min-level :warn
         :rate-limit nil
         :output-fn :inherit
         :fn (fn [{:keys [level] :as data}]
               (->> {:level level}
                    (dd/increment datadog-spec "wizard-tags.logs")))}}}))

(defn wrap-stacktrace
  [handler]
  (fn [request]
    (try
      (handler request)
      (catch Throwable e
        (timbre/error e)
        (throw e)))))
