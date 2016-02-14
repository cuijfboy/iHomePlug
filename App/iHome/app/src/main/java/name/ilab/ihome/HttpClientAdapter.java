package name.ilab.ihome;

import android.content.Context;

import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.BinaryHttpResponseHandler;
import com.loopj.android.http.FileAsyncHttpResponseHandler;
import com.loopj.android.http.ResponseHandlerInterface;
import com.loopj.android.http.TextHttpResponseHandler;

import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.UnrecoverableKeyException;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import cz.msebera.android.httpclient.Header;
import cz.msebera.android.httpclient.conn.ssl.SSLSocketFactory;
import cz.msebera.android.httpclient.entity.StringEntity;
import cz.msebera.android.httpclient.message.BasicHeader;
import name.ilab.http.HttpMethod;
import name.ilab.http.IHttpClient;
import name.ilab.http.IHttpRequest;

/**
 * <h3>HttpClient适配器</h3>
 * <p>
 * 用于实现HTTP访问功能,结合第三方组件实现具体功能.
 * <p>
 * Created by cuijfboy on 15/12/15.
 */
public class HttpClientAdapter implements IHttpClient {

    /**
     * 程序上下文
     */
    private Context context;

    /**
     * 第三方HTTP客户端实例
     */
    private AsyncHttpClient client;

    /**
     * 构造器
     *
     * @param context 程序上下文
     */
    public HttpClientAdapter(Context context) {
        this.context = context;
        Utils.logger.info("HttpClientAdapter initialized!");
    }

    /**
     * 返回第三方HTTP客户端实例,如果未创建,就创建它
     *
     * @return 第三方HTTP客户端实例
     */
    public synchronized AsyncHttpClient obtainClient() {
        if (client != null) {
            return client;
        }
        try {
            KeyStore keyStore = KeyStore.getInstance(KeyStore.getDefaultType());
            keyStore.load(null, null);
            MySSLSocketFactory mySSLSocketFactory = new MySSLSocketFactory(keyStore);
            mySSLSocketFactory.setHostnameVerifier(MySSLSocketFactory.ALLOW_ALL_HOSTNAME_VERIFIER);
            client = new AsyncHttpClient();
            client.setSSLSocketFactory(mySSLSocketFactory);
        } catch (KeyStoreException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (CertificateException e) {
            e.printStackTrace();
        } catch (KeyManagementException e) {
            e.printStackTrace();
        } catch (UnrecoverableKeyException e) {
            e.printStackTrace();
        }
        return client;
    }

    /**
     * 发起HTTP请求
     *
     * @param request HTTP请求实例
     */
    @Override
    public void request(IHttpRequest request) {
        Utils.logger.info(String.format("REQUEST : %s", request.toString()));
        switch (request.getResponseType()) {
            case TEXT:
                requestText(request);
                break;
            case FILE:
                requestFile(request);
                break;
            case BINARY:
                requestBinary(request);
            default:
                Utils.logger.warning(String.format("ResponseType \"%s\" is not implemented yet !",
                        request.getMethod().name()));
                responseNull(request);
                break;
        }
    }

    /**
     * 请求文本内容
     *
     * @param request HTTP请求实例对象
     */
    private void requestText(final IHttpRequest request) {
        if (!checkHttpMethod(request,
                HttpMethod.GET, HttpMethod.POST, HttpMethod.PUT, HttpMethod.DELETE)) {
            return;
        }
        TextHttpResponseHandler responseHandler = new TextHttpResponseHandler() {
            @Override
            public void onSuccess(int statusCode, Header[] headers, String responseString) {
                Map<String, String> headerMap = generateHeaderMap(headers);
                Utils.logger.info(String.format(
                        "SUCCESS : statusCode = %d responseType=%s header = %s body = %s",
                        statusCode, request.getResponseType(), headerMap, responseString));
                request.onResponse(statusCode, headerMap, responseString);
            }

            @Override
            public void onFailure(int statusCode, Header[] headers, String responseString, Throwable throwable) {
                Map<String, String> headerMap = generateHeaderMap(headers);
                Utils.logger.info(String.format(
                        "FAILURE : statusCode = %d responseType=%s header = %s body = %s \n throwable = %s",
                        statusCode, request.getResponseType(), headerMap, responseString, throwable));
                request.onResponse(statusCode, headerMap, responseString);
            }
        };
        request(context, request, "UTF-8", "application/json", responseHandler);
    }

    /**
     * 请求文件内容,仅支持下载单一文件
     *
     * @param request HTTP请求实例对象
     */
    private void requestFile(final IHttpRequest request) {
        if (!checkHttpMethod(request, HttpMethod.GET, HttpMethod.POST)) {
            return;
        }
        FileAsyncHttpResponseHandler responseHandler = new FileAsyncHttpResponseHandler(
                new File(request.getFileSavePath())) {
            @Override
            public void onSuccess(int statusCode, Header[] headers, File file) {
                Map<String, String> headerMap = generateHeaderMap(headers);
                Utils.logger.info(String.format(
                        "SUCCESS : statusCode = %d responseType=%s header = %s file = %s",
                        statusCode, request.getResponseType(), headerMap, file));
                request.onResponse(statusCode, headerMap, file);
            }

            @Override
            public void onFailure(int statusCode, Header[] headers, Throwable throwable, File file) {
                Map<String, String> headerMap = generateHeaderMap(headers);
                Utils.logger.info(String.format(
                        "FAILURE : statusCode = %d responseType=%s header = %s file = %s \n throwable = %s",
                        statusCode, request.getResponseType(), headerMap, file, throwable));
                request.onResponse(statusCode, headerMap, file);
            }
        };
        request(request, responseHandler);
    }

    /**
     * 请求二进制内容
     *
     * @param request HTTP请求实例对象
     */
    private void requestBinary(final IHttpRequest request) {
        if (!checkHttpMethod(request, HttpMethod.GET, HttpMethod.POST)) {
            return;
        }
        BinaryHttpResponseHandler responseHandler = new BinaryHttpResponseHandler() {
            @Override
            public void onSuccess(int statusCode, Header[] headers, byte[] binaryData) {
                Map<String, String> headerMap = generateHeaderMap(headers);
                Utils.logger.info(String.format(
                        "SUCCESS : statusCode = %d responseType=%s header = %s binaryData = %s",
                        statusCode, request.getResponseType(), headerMap, binaryData));
                request.onResponse(statusCode, headerMap, binaryData);
            }

            @Override
            public void onFailure(int statusCode, Header[] headers, byte[] binaryData, Throwable error) {
                Map<String, String> headerMap = generateHeaderMap(headers);
                Utils.logger.info(String.format(
                        "FAILURE : statusCode = %d responseType=%s header = %s binaryData = %s \n error = %s",
                        statusCode, request.getResponseType(), headerMap, binaryData, error));
                request.onResponse(statusCode, headerMap, binaryData);
            }
        };
        request(request, responseHandler);
    }

    /**
     * 发起请求
     *
     * @param context         程序上下文
     * @param request         HTTP请求实例对象
     * @param charset         字符集
     * @param contentType     内容类型
     * @param responseHandler HTTP请求的响应处理器
     */
    private void request(Context context, IHttpRequest request, String charset, String contentType,
                         ResponseHandlerInterface responseHandler) {
        switch (request.getMethod()) {
            case GET:
                obtainClient().get(context, request.getUrl(),
                        generateHeaderArray(request.getHeader()),
                        null, responseHandler);
                break;
            case POST:
                obtainClient().post(context, request.getUrl(),
                        generateHeaderArray(request.getHeader()),
                        new StringEntity(request.getBody(), charset),
                        contentType, responseHandler);
                break;
            case PUT:
                obtainClient().put(context, request.getUrl(),
                        generateHeaderArray(request.getHeader()),
                        new StringEntity(request.getBody(), charset),
                        contentType, responseHandler);
            case DELETE:
                obtainClient().delete(context, request.getUrl(),
                        generateHeaderArray(request.getHeader()),
                        null, responseHandler);
            default:
                warnNotImplementedHttpMethod(request);
                responseNull(request);
                break;
        }
    }

    /**
     * 发起请求
     *
     * @param request         HTTP请求实例对象
     * @param responseHandler HTTP请求的响应处理器
     */
    private void request(IHttpRequest request, ResponseHandlerInterface responseHandler) {
        switch (request.getMethod()) {
            case GET:
                obtainClient().get(request.getUrl(), responseHandler);
                break;
            case POST:
                obtainClient().post(request.getUrl(), responseHandler);
                break;
            default:
                warnNotImplementedHttpMethod(request);
                responseNull(request);
                break;
        }
    }

    /**
     * 检查请求的HTTP方法是否被支持
     *
     * @param request HTTP请求实例对象
     * @param methods 被支持的HTTP方法列表
     * @return 请求的HTTP方法是否被支持
     */
    private boolean checkHttpMethod(IHttpRequest request, HttpMethod... methods) {
        if (request.getMethod() != null && methods != null) {
            for (HttpMethod method : methods) {
                if (request.getMethod() == method) {
                    return true;
                }
            }
        }
        warnNotImplementedHttpMethod(request);
        responseNull(request);
        return false;
    }

    /**
     * 转换HTTP头数据结构
     *
     * @param headers 被转换的HTTP头数据
     * @return 换换后的HTTP头数据
     */
    public Map<String, String> generateHeaderMap(Header[] headers) {
        Map<String, String> headerMap = Collections.emptyMap();
        if (headers != null && headers.length > 0) {
            headerMap = new HashMap<>();
            for (Header header : headers) {
                headerMap.put(header.getName(), header.getValue());
            }
        }
        return headerMap;
    }

    public Header[] generateHeaderArray(Map<String, String> headerMap) {
        if (headerMap == null || headerMap.isEmpty()) {
            return null;
        }
        Header[] headers = new Header[headerMap.size()];
        int i = 0;
        for (Map.Entry<String, String> entry : headerMap.entrySet()) {
            headers[i++] = new BasicHeader(entry.getKey(), entry.getValue());
        }
        return headers;
    }

    /**
     * 打印日志,报警未实现的HTTP方法
     *
     * @param request HTTP请求对象
     */
    private void warnNotImplementedHttpMethod(IHttpRequest request) {
        Utils.logger.warning(String.format(
                "Http method \"%s\" is not implemented yet for responseType \"%s\" !",
                request.getMethod(), request.getResponseType()));
    }

    /**
     * 调用回调,返回null数据
     *
     * @param request HTTP请求对象
     */
    private void responseNull(IHttpRequest request) {
        Utils.logger.warning(String.format(
                "Response null value for request : url = %s method = %s responseType = %s !",
                request.getUrl(), request.getMethod(), request.getResponseType()));
        switch (request.getResponseType()) {
            case TEXT:
                String nullString = null;
                request.onResponse(0, null, nullString);
                break;
            case FILE:
                File nullFile = null;
                request.onResponse(0, null, nullFile);
                break;
            case BINARY:
                byte[] nullBinary = null;
                request.onResponse(0, null, nullBinary);
                break;
            default:
                break;
        }
    }

    private class MySSLSocketFactory extends SSLSocketFactory {

        SSLContext sslContext = SSLContext.getInstance("TLS");

        public MySSLSocketFactory(KeyStore truststore)
                throws NoSuchAlgorithmException, KeyManagementException,
                KeyStoreException, UnrecoverableKeyException {
            super(truststore);

            TrustManager trustManager = new X509TrustManager() {
                @Override
                public void checkClientTrusted(X509Certificate[] chain, String authType)
                        throws CertificateException {

                }

                @Override
                public void checkServerTrusted(X509Certificate[] chain, String authType)
                        throws CertificateException {

                }

                @Override
                public X509Certificate[] getAcceptedIssuers() {
                    return new X509Certificate[0];
                }
            };
            sslContext.init(null, new TrustManager[]{trustManager}, null);
        }

        @Override
        public Socket createSocket() throws IOException {
            return sslContext.getSocketFactory().createSocket();
        }

        @Override
        public Socket createSocket(Socket socket, String host, int port, boolean autoClose)
                throws IOException, UnknownHostException {
            return sslContext.getSocketFactory().createSocket(socket, host, port, autoClose);
        }
    }

}