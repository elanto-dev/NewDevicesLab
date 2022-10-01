using System;
using System.Collections.Generic;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Text;
using Microsoft.IdentityModel.Tokens;

namespace Identity
{
    public static class JwtHelper
    {
        public static string GenerateJwt(IEnumerable<Claim> claims, string key, string issuer, int expiresInDays)
        {
            var signingKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(key));
            var signingCredentials = new SigningCredentials(signingKey, SecurityAlgorithms.HmacSha256);
            var expires = DateTime.Now.AddDays(expiresInDays);

            var token = new JwtSecurityToken(
                issuer: issuer,
                audience: issuer,
                claims: claims,
                expires: expires,
                signingCredentials: signingCredentials
            );


            return new JwtSecurityTokenHandler().WriteToken(token);
        }
    }
}