using System.ComponentModel.DataAnnotations;
using System.Threading.Tasks;
using Domain.Identity;
using Identity;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Options;
using ShelfWebApp.PublicAPI.Identity;

namespace ShelfWebApp.Controllers
{
    [Route("api/[action]")]
    [ApiController]
    public class UsersController : ControllerBase
    {
        private readonly SignInManager<User> _signInManager;
        private readonly UserManager<User> _userManager;
        private readonly IOptions<JWT> _jwtOptions;
        private readonly IConfiguration _configuration;


        public UsersController(SignInManager<User> signInManager, IConfiguration configuration, UserManager<User> userManager, IOptions<JWT> jwtOptions)
        {
            _signInManager = signInManager;
            _configuration = configuration;
            _userManager = userManager;
            _jwtOptions = jwtOptions;
        }

        /// <summary>
        /// Login action that logs user in with required parameters.
        /// </summary>
        /// <param name="model">Login model includes email and password</param>
        /// <returns></returns>
        [HttpPost]
        public async Task<ActionResult<string>> Login([FromBody] LoginDto model)
        {

            var appUser = await _userManager.FindByEmailAsync(model.Email);

            if (appUser == null)
            {
                return StatusCode(403);
            }

            // do not log user in, just check that the password is ok
            var result = await _signInManager.CheckPasswordSignInAsync(appUser, model.Password, false);

            if (result.Succeeded)
            {
                // create claims based user 
                var claimsPrincipal = await _signInManager.CreateUserPrincipalAsync(appUser);

                // get the Json Web Token
                var jwt = JwtHelper.GenerateJwt(
                    claimsPrincipal.Claims,
                    _configuration["JWT:Key"],
                    _configuration["JWT:Issuer"],
                    int.Parse(_configuration["JWT:ExpireDays"]));

                return Ok(new { token = jwt });
            }

            return StatusCode(403);
        }

        /// <summary>
        /// Register action creates a new user with given parameters.
        /// </summary>
        /// <param name="model">Register model with parameters</param>
        /// <returns></returns>
        [HttpPost]
        public async Task<ActionResult<string>> Register([FromBody] RegisterDto model)
        {
            if (ModelState.IsValid)
            {
                var appUser = new User
                {
                    UserName = model.UserName,
                    Email = model.Email,
                    EmailConfirmed = true
                };
                var result = await _userManager.CreateAsync(appUser, model.Password);
                if (result.Succeeded)
                {
                    // create claims based user 
                    var claimsPrincipal = await _signInManager.CreateUserPrincipalAsync(appUser);

                    // get the Json Web Token
                    // get the Json Web Token
                    var jwt = JwtHelper.GenerateJwt(
                        claimsPrincipal.Claims,
                        _configuration["JWT:Key"],
                        _configuration["JWT:Issuer"],
                        int.Parse(_configuration["JWT:ExpireDays"]));

                    return Ok(new { token = jwt });

                }
                return StatusCode(406); //406 Not Acceptable
            }

            return StatusCode(400); //400 Bad Request
        }
    }
}
