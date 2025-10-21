// try to make a request builder
// note that request often have different required/optional params
// this makes a single constructor hard, which is why we use a request builder instead

// 1. Create the product: In this case it's a request class
// the request has: URL, HTTP method, headers (might be many), body (optional), query params, timeout
// 2. Create a builder: The builder should create abstract initialization methods
// 3. Create a specifc builder type for an implementation, say get request builder. Should return the builder to chain multiple things together
// 4. Consider having a director to orchestrate the building

#include <string>
#include <vector>
#include <iostream>

enum class HTTPMethod
{
    Get,
    Post,
    Put
};


class Request
{
public:
    std::string define()
    {
        std::string method_string;
        switch(method_)
        {
            case HTTPMethod::Get:
                method_string = "[GET]";
                break;
            case HTTPMethod::Post:
                method_string = "[POST]";
                break;
            case HTTPMethod::Put:
                method_string = "[PUT]";
                break;
            default:
                throw "Error: Invalid method used";
        }

        std::string return_str = method_string + ": " + url_ + " Headers: ";

        for (const auto& header: headers_)
        {
            return_str += " " + header;
        }

        return_str += "Timeout " + std::to_string(timeout_);
        return return_str;
    }

    void setUrl(const std::string& url)
    {
        url_ = url;
    }

    void setMethod(const HTTPMethod& method)
    {
        method_ = method;
    }

private:
    std::string url_;
    HTTPMethod method_;
    std::vector<std::string> headers_;
    float timeout_;
};

class RequestBuilder
{
public:
    virtual void setUrl(const std::string& url) = 0;
    virtual void setMethod() = 0;

    std::string show()
    {
        return request_.define();
    }

protected:
    // might also consider using a shared pointer here; weak 'has-a' relationship
    Request request_;
};

class GetRequestBuilder: public RequestBuilder
{
public:
    void setUrl(const std::string& url) override
    {
        request_.setUrl(url);
    }
    
    void setMethod() override
    {
        request_.setMethod(HTTPMethod::Get);
    }
};

class Director
{
public:
    std::string setup(RequestBuilder& builder, const std::string& url)
    {
        builder.setUrl(url);
        builder.setMethod();
        return builder.show();
    }
};

int main()
{
    auto get_req_builder = GetRequestBuilder();
    auto director = Director();
    const auto output = director.setup(get_req_builder, "http://test.com");
    std::cout << output << std::endl;

}