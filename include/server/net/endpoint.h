#ifndef ENDPOINT_H
#define ENDPOINT_H

class Endpoint {
private:
  std::string host_;
  uint16_t port_;
public:
  explicit Endoint(std::string host, std::uint16_t port);
  const &std::string host() const noexcept;
  uint16_t port() const noexcept;
  std::string to_string() const;

};

#endif // !ENDPOINT_H
